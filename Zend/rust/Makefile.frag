# Makefile fragment for Zend Rust components
# Variables will be set by configure and substituted into Makefile

# Build the Rust library
Zend/rust/target/release/libzend_rust.a: Zend/rust/Cargo.toml Zend/rust/build.rs Zend/rust/src/lib.rs Zend/rust/src/memory/mod.rs Zend/rust/src/memory/impls.rs Zend/rust/src/base64/mod.rs Zend/rust/src/base64/impls.rs Zend/rust/src/baseconv/mod.rs Zend/rust/src/baseconv/impls.rs
	@echo "Building Rust library..."
	cd Zend/rust && cargo build --release
	@echo "Rust library built successfully"

# Generate C header from Rust code using cbindgen
Zend/rust/zend_rust.h: Zend/rust/Cargo.toml Zend/rust/src/lib.rs Zend/rust/src/memory/mod.rs Zend/rust/src/memory/impls.rs Zend/rust/src/base64/mod.rs Zend/rust/src/base64/impls.rs Zend/rust/src/baseconv/mod.rs Zend/rust/src/baseconv/impls.rs
	@echo "Generating C header from Rust code..."
	cd Zend/rust && cbindgen --config cbindgen.toml --output zend_rust.h
	@echo "C header generated successfully"

# Make zend_alloc.lo depend on Rust library and header
Zend/zend_alloc.lo: Zend/rust/target/release/libzend_rust.a Zend/rust/zend_rust.h

# Make base64.lo depend on Rust library and header
ext/standard/base64.lo: Zend/rust/target/release/libzend_rust.a Zend/rust/zend_rust.h

# Make math.lo depend on Rust library and header
ext/standard/math.lo: Zend/rust/target/release/libzend_rust.a Zend/rust/zend_rust.h

# Clean targets
rust-clean:
	@if test -d Zend/rust; then cd Zend/rust && cargo clean; fi

# Test targets
rust-test:
	cd Zend/rust && cargo test --release

# Add to main clean target
clean: rust-clean

# Phony targets
.PHONY: rust-clean rust-test