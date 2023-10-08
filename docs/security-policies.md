# PHP Security Policies and Process

> [!IMPORTANT]
> This is a meta document discussing PHP security policies and processes. For the actual
> PHP security policy, see the PHP [Vulnerability Disclosure Policy][] document.

## PHP.net security.txt file

PHP.net includes a [security.txt][] file that complements the
[Vulnerability Disclosure Policy][], aiding security vulnerability disclosure.
This file implements the standard defined in [RFC 9116][], and more information
is available at <https://securitytxt.org>.

RFC 9116 requires an `Expires` field in `security.txt`, and its recommendation
is for the `Expires` field to be less than a year in the future. This provides
security researchers with confidence they are using our most up-to-date
reporting policies. To facilitate yearly updates to the `Expires` field and
ensure freshness of the information in `security.txt`, the PHP release managers
[update the `Expires` field as part of the X.Y.0 GA release][expires-update].

From time-to-time, we may update `security.txt` with new information, outside
of the yearly changes to the `Expires` field.

### Making changes to security.txt

All changes to `security.txt` must be signed by a PHP release manager for a
[currently supported version of PHP][supported-versions] (at the time of the
changes). Release managers are the most logical choice for signing this file,
since we already [publish their PGP keys][rm-pgp-keys].

To make changes to `security.txt`:

1. Go to your local clone of [web-php][].

   ```bash
   cd /path/to/web-php/.well-known
   ```

2. Remove the PGP signature that wraps the body of `security.txt`:

   ```bash
   gpg --decrypt --output security.txt security.txt
   ```

   > [!NOTE]
   > To "decrypt" `security.txt`, you will need the public key of the release
   > manager who last signed it in your GPG keychain.

3. Make and save your changes to this file, e.g., update the `Expires` timestamp.

   There should be a "Signed by" comment in the file that looks similar to this:

   ```
   # Signed by Ben Ramsey <ramsey@php.net> on 2023-09-28.
   ```

   Update this line with your name, the email address associated with the key
   you're using to sign the file, and the current date.

4. Sign your changes:

   ```bash
   gpg --clearsign --local-user YOU@php.net --output security.txt.asc security.txt
   ```

   > [!WARNING]
   > You cannot use `--output` to output the signature to the same file as the
   > input file or `gpg` will result in a signature wrapped around empty content.

5. Last, replace `security.txt` with `security.txt.asc` and commit your changes:

   ```bash
   mv security.txt.asc security.txt
   git commit security.txt
   ```

> [!NOTE]
> You may verify the signature with the following command:
>
> ```bash
> gpg --verify security.txt
> ```

[security.txt]: https://www.php.net/.well-known/security.txt
[vulnerability disclosure policy]: https://github.com/php/php-src/security/policy
[rfc 9116]: https://www.rfc-editor.org/rfc/rfc9116
[expires-update]: release-process.md#preparing-for-the-initial-stable-version-php-xy0
[supported-versions]: https://www.php.net/supported-versions.php
[rm-pgp-keys]: https://www.php.net/gpg-keys.php
[web-php]: https://github.com/php/web-php
