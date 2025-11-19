# Rust Integration - Build Instructions

Proof of concept: Rust implementations for Zend Memory Management (`zend_strndup`, `malloc`, `free`, `realloc`) and base functions (`base64_encode`, `base64_decode`, `base_convert`).

## Prerequisites

- Rust toolchain (rustc, cargo)
- cbindgen (optional): `cargo install cbindgen`

## Build with Rust

```bash
./buildconf --force
./configure --enable-zend-rust
make -j$(nproc)
```

## Build without Rust

```bash
./buildconf --force
./configure
make -j$(nproc)
```

## Verify

```bash
# Check if Rust is enabled
./sapi/cli/php -i | grep -i rust

# Or check config header
grep HAVE_ZEND_RUST main/php_config.h
```

## Test

```bash
# Run Rust tests
cd Zend/rust && cargo test

# Run PHP tests
make test
```
