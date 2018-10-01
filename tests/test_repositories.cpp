//
// Created by Peyman Mortazavi on 9/21/18.
//

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "garlicconfig.h"


using namespace boost::filesystem;
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
  path test_directory{"testdata"};

  void SetUp() override {
    create_directory(this->test_directory);
  }

  void TearDown() override {
    remove_all(this->test_directory);
  }
};

TEST_F(FileRepositoryTests, BasicTests) {
  auto file_repository = make_unique<FileConfigRepository>(this->test_directory.string());
  run_basic_repository_tests(*file_repository);
}


// Memory Repository
TEST(MemoryRepositoryTests, BasicTests) {
  auto memory_repository = make_unique<MemoryConfigRepository>();
  run_basic_repository_tests(*memory_repository);
}