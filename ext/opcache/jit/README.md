Opcache JIT
===========

This is the implementation of Opcache's JIT (Just-In-Time compiler),
This converts the PHP Virtual Machine's opcodes into Intermediate
Representation and uses [IR - Lightweight JIT Compilation Framework](https://github.com/dstogov/ir)
to produce optimized native code. The necessary part of the IR
Framework is embedded into php-src.

Running tests of the JIT
------------------------

Then, to test the JIT, e.g. with opcache.jit=tracing, an example command
based on what is used to test in CI:

```
make test TESTS="-d opcache.jit_buffer_size=16M -d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.protect_memory=1 -d opcache.jit=tracing --repeat 2 --show-diff -j$(nproc) ext/opcache Zend"
```

- `opcache.jit_buffer_size=16M` enables the JIT in tests by providing 16 megabytes of
  memory to use with the JIT to test with.
- `opcache.protect_memory=1` will detect writing to memory that is meant to be
  read-only, which is sometimes the cause of opcache bugs.
- `--repeat 2` is optional, but used in CI since some JIT bugs only show up after processing a
  request multiple times (the first request compiles the trace and the second executes it)
- `-j$(nproc)` runs as many workers to run tests as there are CPUs.
- `ext/opcache/` and `Zend` are the folders with the tests to run, in this case opcache
  and the Zend engine itself.  If no folders are provided, all tests are run.

When investigating test failures such as segmentation faults,
configuring the build of php with `--enable-address-sanitizer` to enable
[AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) is often useful.

Some of the time, adding `-m --show-mem` to the `TESTS` configuration is also useful to test with [valgrind](https://valgrind.org/) to detect out of bounds memory accesses.
Using valgrind is slower at detecting invalid memory read/writes than AddressSanitizer when running large numbers of tests, but does not require rebuilding php.

Note that the JIT supports 3 different architectures: `X86_64`, `i386`, and `arm64`.

Miscellaneous
-------------

### How to build 32-bit builds on x86_64 environments

Refer to [../../../.github/workflows/push.yml](../../../.github/workflows/push.yml) for examples of
dependencies to install.

If you are running this natively (outside of Docker or a VM):

- Consider running in docker/a VM instead if you are unfamiliar with this.
- Avoid purging packages.
- Avoid `-y` - if the package manager warns you that the dependencies conflict
  then **don't** try to force install them.

#### Prerequisites for 32-bit builds

This assumes you are using a Debian-based Linux distribution and have already
set up prerequisites for regular development.

```
sudo dpkg --add-architecture i386
sudo apt-get update -y
# As well as anything else from .github/actions/apt-x32/action.yml that you're testing locally
sudo apt-get install \
    gcc-multilib g++-multilib \
    libxml2-dev:i386 \
    libc6:i386
```

#### Compiling 32-bit builds

This assumes you are using a Debian-based Linux distribution and have already
set up prerequisites for 32-bit development.

```
export LDFLAGS=-L/usr/lib/i386-linux-gnu
export CFLAGS='-m32'
export CXXFLAGS='-m32'
export PKG_CONFIG=/usr/bin/i686-linux-gnu-pkg-config
./configure --disable-all --enable-opcache --build=i686-pc-linux-gnu
make -j$(nproc)
```

#### Running tests of the JIT on 32-bit builds

See the section "Running tests of the JIT".

### Testing the jit with arm64 on x86 computers

https://www.docker.com/blog/faster-multi-platform-builds-dockerfile-cross-compilation-guide/
may be useful for local development.

Note that this is slower than compiling and testing natively.

```
# After following steps in https://www.docker.com/blog/faster-multi-platform-builds-dockerfile-cross-compilation-guide/
cp .gitignore .dockerignore
echo .git >> .dockerignore

docker build --network=host -t php-src-arm64-example -f ext/opcache/jit/Dockerfile.arm64.example .
docker run -it --rm php-src-arm64-example
```

Then, the docker image can be used to run tests with `make test`.
For example, to test `ext/opcache` in parallel with the tracing JIT enabled:

```
docker run -it php-src-arms-example make test TESTS="-d opcache.jit_buffer_size=16M -d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.protect_memory=1 -d opcache.jit=tracing --repeat 2 --show-diff -j$(nproc) ext/opcache"
```
