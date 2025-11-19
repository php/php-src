use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rustc-link-lib=c");

    // For now, skip bindgen during release builds to avoid configuration issues
    // The bindings are mostly for future use when integrating with PHP's error handling
    #[cfg(debug_assertions)]
    {
        let bindings = bindgen::Builder::default()
            .header("wrapper.h")
            .allowlist_function("zend_error_noreturn")
            .allowlist_function("zend_out_of_memory")
            .allowlist_function("malloc")
            .allowlist_function("free")
            .allowlist_function("memcpy")
            .allowlist_type("size_t")
            .clang_arg("-I..")
            .clang_arg("-I../..")
            .clang_arg("-I../../main")
            .clang_arg("-I../../TSRM")
            .clang_arg("-DHAVE_ALLOCA_H=1")
            .clang_arg("-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1")
            .clang_arg("-DSIZEOF_SIZE_T=8") // Assume 64-bit for now
            .clang_arg("-DZEND_MM_ALIGNMENT=8")
            .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
            .generate()
            .expect("Unable to generate bindings");

        let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
        bindings
            .write_to_file(out_path.join("bindings.rs"))
            .expect("Couldn't write bindings!");
    }

    // For release builds or when bindgen is skipped, create empty bindings
    #[cfg(not(debug_assertions))]
    {
        let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
        std::fs::write(
            out_path.join("bindings.rs"),
            "// Placeholder bindings for release build\n",
        )
        .expect("Couldn't write bindings!");
    }

    println!("cargo:rerun-if-changed=../zend.h");
    println!("cargo:rerun-if-changed=../zend_types.h");
    println!("cargo:rerun-if-changed=../zend_errors.h");
    println!("cargo:rerun-if-changed=../zend_alloc.h");
}
