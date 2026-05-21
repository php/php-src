# Reporting Security Issues

Please report security vulnerabilities on GitHub at:
<https://github.com/php/php-src/security/advisories/new>

If for some reason you cannot use the form at GitHub, or you need to talk to
somebody about a PHP security issue that might not be a bug report, please write
to <security@php.net>.

Vulnerability reports remain private until published. When published, you will
be credited as a contributor, and your contribution will reflect the MITRE
Credit System.

# Classification

Issues commonly reported that are _not_ considered security issues include (but
are not limited to):

- Invocation of specially crafted, malicious code intended to cause memory
  violations. This commonly includes malicious error handlers, destructors or
  `__toString()` functions. PHP does not offer sandboxing, and the execution of
  untrusted code is always considered unsafe.

- Passing malicious arguments to functions clearly not intended to receive
  unsanitized values, e.g. `mysqli_query()`. `escapeshellarg()` on the other
  hand should clearly be hardened against unsafe inputs.

- The use of legacy APIs or settings known to be insecure.

- The use of FFI.

- `open_basedir` or `disable_functions` bypasses.

# Vulnerability Policy

Our full policy is described at
https://github.com/php/policies/blob/main/security-classification.rst
