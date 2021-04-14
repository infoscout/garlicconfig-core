# GarlicConfig

[![CircleCI](https://circleci.com/gh/infoscout/garlicconfig-core.svg?style=svg&circle-token=0fa1ed375c058bb98d7d2dc3e1bf871a9c6706fd)](https://circleci.com/gh/infoscout/garlicconfig-core)


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


### Using Docker Compose

You can build all the required tools and dependencies using:

```bash
$ docker-compose build
```

And then compile and test using:

```bash
$ docker-compose run
```
