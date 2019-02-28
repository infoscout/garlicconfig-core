//
// Created by Peyman Mortazavi on 9/21/18.
//

#include <cstdlib>

#include <gtest/gtest.h>

#include "garlicconfig.h"


using namespace garlic;
using namespace std;
using namespace ::testing;


void run_basic_repository_tests(ConfigRepository& repo) {
  // No config should be existing at the moment.
  ASSERT_EQ(repo.list_configs().size(), 0);

  // Adding two configs: string_config and binary_config
  auto original_text = "string content";
  repo.save("string_config", [&original_text](ostream& output_stream) {
    output_stream << original_text;
  });

  char original_bytes[] {0x0, 0x1, 0xa, 0xb, 86};
  repo.save("binary_config", [&original_bytes](ostream& output_stream) {
    output_stream.write(original_bytes, sizeof(original_bytes));
  });

  // Two configs should be added.
  auto all_configs = repo.list_configs();
  ASSERT_EQ(all_configs.size(), 2);
  ASSERT_TRUE(all_configs.find("string_config") != all_configs.end());
  ASSERT_TRUE(all_configs.find("binary_config") != all_configs.end());

  ASSERT_EQ(string(istreambuf_iterator<char>(repo.retrieve("string_config")->rdbuf()), {}), "string content");
  char buffer[5];
  repo.retrieve("binary_config")->read(buffer, sizeof(buffer));
  for (auto index = 0; index < 5; index++) {
    ASSERT_EQ(buffer[index], original_bytes[index]);
  }

  // Make sure undefined configs throw the config not found exception.
  ASSERT_THROW(repo.retrieve("none_existing_config"), ConfigNotFound);
}

// File Repository
class FileRepositoryTests : public Test {
public:
  string test_directory{"testdata"};

  void SetUp() override {
    string cmd = "mkdir " + test_directory;
    std::system(cmd.c_str());
  }

  void TearDown() override {
    string cmd = "rm -fr " + test_directory;
    std::system(cmd.c_str());
  }
};

TEST_F(FileRepositoryTests, BasicTests) {
  auto file_repository = unique_ptr<FileConfigRepository>(new FileConfigRepository(this->test_directory));
  run_basic_repository_tests(*file_repository);
}


// Memory Repository
TEST(MemoryRepositoryTests, BasicTests) {
  auto memory_repository = unique_ptr<MemoryConfigRepository>(new MemoryConfigRepository());
  run_basic_repository_tests(*memory_repository);
}