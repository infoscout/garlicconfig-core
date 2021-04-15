# GarlicConfig

C++ library to quickly read, write and merge garlic config layers. Note that this library does not include any validation logic.

## Integration
You can build from the source and use any compiler you choose to use and use the generated library to build on top of it. However, if you're only thinknig of integrating with this code as is, using `cget` to retrieve this is highly recommended. In order to use `cget` simply use this command.

`$ cget install infoscout/garlicconfig-core`

If you'd like to retreive shared libraries, use:
`$ cget install infoscout/garlicconfig-core -DBUILD_SHARED_LIB=ON`

Otherwise, you should be able to compile from the source after either satisfying all the dependencies or using cget to retrieve depending by calling `init.sh` or simply calling.

`$ cget install`

You just have to make sure cget is initialized to use C++11 and set your preferred build type, shared or static.

`$ cget init --std=c++11`

## Development
You need to make sure you have cGet, CMake 3.11 and a GNU Compiler.

### Local

Install cget and cmake if you don't already have them:

```bash
$ pip install cget cmake
```

Install all dependencies:

```bash
$ ./init.sh
```

Then you can make a build directory and initialize the CMake project:

```bash
$ mkdir build && cd build && cmake ..
```

Now you can build and test from the `build` directory:

```bash
$ make && make test
```

> NOTE: For folks on macOS using AppleClang, you need to do this prior to building:

```bash
$ export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$(pwd)/cget/lib:$(pwd)/cget/lib64
```


### Using Docker Compose

You can build all the required tools and dependencies using:

```bash
$ docker-compose build
```

And then compile and test using:

```bash
$ docker-compose run
```
