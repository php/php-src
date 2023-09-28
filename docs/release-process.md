# PHP Release Process

A release manager's role includes making packaged source code from the canonical
repository available according to the release schedule.

The release schedule for each version is published on the
[PHP wiki](https://wiki.php.net):

- [PHP 8.3](https://wiki.php.net/todo/php83)
- [PHP 8.2](https://wiki.php.net/todo/php82)
- [PHP 8.1](https://wiki.php.net/todo/php81)
- [PHP 8.0](https://wiki.php.net/todo/php80)

The PHP project publishes builds every two weeks.

We publish [general availability][] (GA) releases for major and minor versions of
PHP on the fourth Thursday of November each year. Following the GA release, we
publish patch-level releases every four weeks, with at least one release
candidate (RC) published two weeks before each patch-level release.

Each major and minor version undergoes a 24-week pre-release cycle before GA
release. The pre-release cycle begins on the second Thursday of June with the
first alpha release of the new major/minor version. The pre-release cycle
consists of at least:

- 3 alpha releases
- 3 beta releases
- 6 release candidates

Feature freeze for the next major/minor occurs with the first beta release.

We refer to alpha, beta, and RC as *non-stable releases*, while GA are *stable*.

The process of making packaged source available and announcing availability is
explained in detail below. The process differs slightly for non-stable and
stable releases.

New release managers should review [New release manager
checklist](#new-release-manager-checklist) at the end of this document. This
section  explains the procedures for getting ready to begin work on managing PHP
releases.


## General notes and tips

1. Do not release on Fridays, Saturdays, or Sundays as this gives poor lead
   time for downstream consumers adhering to a typical work week.

   Our general procedure is to release on Thursdays, whenever possible.

2. Package two days before a release.

   If the release is to be on Thursday, package on Tuesday. Think about
   timezones as well.

3. Ensure that the relevant tests on CI are green.

   - https://travis-ci.com/github/php/php-src
   - https://cirrus-ci.com/github/php/php-src
   - https://github.com/php/php-src/actions

   > 💡 **Tip** \
   > We recommend checking the build status a couple of days before packaging day
   > to allow enough time to investigate failures, communicate with the authors,
   > and commit any fixes.
   >
   > Check the CI status for your branch periodically and resolve the failures
   > ASAP.
   >
   > See more in https://wiki.php.net/rfc/travis_ci.

4. Follow all steps to the letter.

   > 💡 **Tip** \
   > When you are unsure about anything, ask a previous RM before proceeding.
   > Ideally, make sure a previous RM is available to answer questions during
   > the first few releases of your version. For the steps related to the
   > `web-php`, `web-qa`, and `web-php-distributions` repositories, try to have
   > someone from the webmaster team on hand.

5. Verify the tags to be extra sure everything was tagged properly.

6. There is a PHP release Docker image https://github.com/sgolemon/php-release
   with forks available to help with releases.

7. Communicate with release managers via release-managers@php.net.

8. References to repositories in this document refer to the canonical source
   located at https://github.com/php.

9. It might be helpful to name your remote repositories something other than
   "origin" to avoid accidentally pushing changes to "origin" with [muscle
   memory][].

10. It might also be helpful to set up conditional includes in your global
    `~/.gitconfig` to set the proper `user.name`, `user.email`, and
    `user.signingKey` values to use with your local PHP repositories. See
    [Conditional Includes For Git Config][] for more information.


## Packaging a non-stable release (alpha/beta/RC)

All releases during the pre-release cycle (alpha/beta/RC) leading up to the GA
release for a version are *non-stable* releases. Following the GA release, all
RCs are *non-stable* releases.

All non-stable releases follow a similar pattern, though pre-GA releases have
slightly different steps. We'll call attention where the steps differ.

1. Check that CI is passing (see above).

2. Run `./scripts/dev/credits` in php-src and commit the changes to the credits
   files in `ext/standard`.

   > 💬 **Hint** \
   > It's rare this script will make any changes, so if you run `git diff`
   > and do not see anything, there's no need to panic. That means there are no
   > changes to the credits files.

3. Check out the *release branch* for this release from the *version branch*.

   > 🔶 **Non-stable version branches: pre-GA** \
   > There is no *version branch* for alpha or beta releases. Instead, treat the
   > main branch as the version branch. You will create a local-only release
   > branch from the main branch. Do not push it!
   >
   > ```shell
   > git checkout -b php-X.Y.0alpha1-local-release-branch upstream/master
   > ```
   >
   > During the first RC release, you will create (and push!) the version
   > branch for the pre-GA release, e.g., `PHP-8.2`. See
   > "[Forking a new version branch](#forking-a-new-version-branch)" below.
   > From this point forward, all pre-GA release branches will be created from
   > this version branch. Again, these release branches are local-only. Do not
   > push them!
   >
   > ```shell
   > git checkout -b php-X.Y.0beta2-local-release-branch upstream/PHP-X.Y
   > ```

   > 🔷 **Non-stable version branches: post-GA** \
   > After GA, you will create (and push) a new *patch-level version branch*
   > along with each non-stable release. For example, if you are building a
   > release for PHP 8.2.8 RC1, you will create the `PHP-8.2.8` patch-level
   > version branch from the `PHP-8.2` version branch.
   >
   > ```shell
   > git checkout -b PHP-X.Y.Z upstream/PHP-X.Y
   > git push upstream PHP-X.Y.Z
   > ```
   >
   > We will use the patch-level version branch to commit any critical bug or
   > security fixes before this version's GA.
   >
   > Then, from the patch-level version branch, you will create another
   > local-only release branch. Do not push this one!
   >
   > ```shell
   > git checkout -b php-X.Y.ZRC1-local-release-branch upstream/PHP-X.Y.Z
   > ```

4. Using your local-only release branch, bump the version numbers in
   `main/php_version.h`, `Zend/zend.h`, `configure.ac`, and possibly
   `NEWS`.

   For examples, see [Update versions for PHP 8.1.0beta3][] (for a pre-GA
   example) or [Update versions for PHP 8.1.6RC1][] along with
   [Update NEWS for PHP 8.1.6RC1][] (for a post-GA example).

   > ⚠️ **Important** \
   > Do not use abbreviations for alpha or beta. Do not use dashes as
   > separators.
   >
   > Do this:
   >
   > ```c
   > #define PHP_VERSION "7.4.22RC1"
   > ```
   >
   > Not this:
   >
   > ```c
   > #define PHP_VERSION "7.4.22-RC1"
   > ```

   > 🗒 **Note** \
   > We update `Zend/zend.h` only when preparing RC and GA releases. We do not
   > update `ZEND_VERSION` for alpha or beta releases.

   > 🚨 **API version bump for pre-GA** \
   > When releasing the *first release candidate* of a pre-GA release, you must
   > also bump the API version numbers in `Zend/zend_extensions.h`,
   > `Zend/zend_modules.h`, and `main/php.h`. See [Prepare for PHP 8.1.0RC1][],
   > for example.
   >
   > The API versions between the alpha, beta, and X.Y.0RCn releases may remain
   > the same, or be bumped as little as possible because PHP extensions need to
   > be rebuilt with each bump.
   >
   > Do *not* bump the API versions after RC1.

5. Compile and run `make test`, with and without ZTS (Zend Thread Safety), using
   the correct Bison and re2c versions, e.g., for PHP 7.4, Bison 3.0.0 and re2c
   0.13.4 are required, as a minimum.

   For example:

   ```shell
   # With ZTS
   make distclean || \
   ./buildconf --force \
       && ./configure --enable-zts --disable-all --enable-debug --enable-opcache --enable-opcache-jit \
       && make -j$(nproc) \
       && make test TEST_PHP_ARGS="-q -j$(nproc)" \
       || ./sapi/cli/php -v

   # Without ZTS
   make distclean || \
   ./buildconf --force \
       && ./configure --disable-all --enable-debug --enable-opcache --enable-opcache-jit \
       && make -j$(nproc) \
       && make test TEST_PHP_ARGS="-q -j$(nproc)" \
       || ./sapi/cli/php -v
   ```

6. After each build, check the output of `./sapi/cli/php -v` to ensure the
   versions match the release.

7. If all is correct, commit the changes to your local-only release branch.

   ```shell
   git add -p
   git commit --gpg-sign=YOURKEYID -m "Update versions for PHP X.Y.ZRCn"
   ```

8. Tag your local-only release branch with the release version.

   ```shell
   git tag -s -u YOURKEYID php-X.Y.ZRCn -m "Tag for php-X.Y.ZRCn"
   ```

9. 🔷 **For post-GA releases only,** switch back to the *version branch* for
   your release (e.g., `PHP-8.2`) and bump the version numbers in
   `main/php_version.h`, `Zend/zend.h`, `configure.ac` and `NEWS`. This prepares
   the version branch for the next version.

   For example, if the RC is `8.2.1RC1` then the version numbers in the version
   branch should be bumped to `8.2.2-dev`. We do this regardless of whether we
   build a new RC to make sure `version_compare()` works correctly. See
   [Bump for 8.1.8-dev][] for a real example.

   Commit the changes to the version branch.

   ```shell
   git add -p
   git commit --gpg-sign=YOURKEYID -m "PHP-X.Y is now for PHP X.Y.Z-dev"
   ```

   > 💡 **Tip** \
   > Version branches (e.g. `PHP-8.1`) will *always* have version numbers in
   > `main/php_version.h`, `Zend/zend.h`, and `configure.ac` that end in `-dev`.
   > Patch-level version branches (e.g. `PHP-8.1.7`) will also *always* have
   > version numbers that end in `-dev` in these files. The main branch (i.e.
   > `master`) will *always* have version numbers that end in `-dev` in these
   > files.
   >
   > Only release tags should have version numbers in these files that do not
   > end in `-dev` (e.g., `8.1.7`, `8.1.7RC1`, `8.2.0alpha1`, etc.).

10. Push the changes to the `php-src`.

    ```shell
    git push upstream php-X.Y.ZRCn # tag name
    git push upstream PHP-X.Y.Z    # patch-level version branch (post-GA only)
    git push upstream PHP-X.Y      # version branch
    ```

    > 🚨 **Attention** \
    > Do not push with `--tags`, as this will push all local tags, including
    > tags you might not wish to push.
    >
    > Local-only release branches should not be pushed!

11. Run the following using the release tag to export the tree, create the
    `configure` script, and build and compress three tarballs (`.tar.gz`,
    `.tar.bz2` and `.tar.xz`).

    ```shell
    ./scripts/dev/makedist php-X.Y.ZRCn
    ```

12. Run the following using the release tag and your GPG key ID to sign the
    tarballs and save the signatures to `php-X.Y.ZRCn.manifest`, which you can
    upload to GitHub and include in the announcement emails.

    ```shell
    ./scripts/dev/gen_verify_stub X.Y.ZRCn YOURKEYID > php-X.Y.ZRCn.manifest
    ```

13. If you have the [GitHub command line tool][] installed, run the following to
    create a public Gist for the manifest file:

    ```shell
    gh gist create --public php-X.Y.ZRCn.manifest
    ```

    Or you may go to https://gist.github.com to create it manually.

14. Copy the tarballs (using scp, rsync, etc.) to your `public_html/` folder on
    downloads.php.net.

    ```shell
    scp php-X.Y.ZRCn.tar.* downloads.php.net:~/public_html/
    ```

    > 💬 **Hint** \
    > If you do not have a `public_html` directory, create it and set its
    > permissions to `0755`.

15. Now the tarballs and signatures may be found at
    `https://downloads.php.net/~yourname/`, e.g. https://downloads.php.net/~derick/.

16. Once the release is tagged, contact the release-managers@php.net distribution
    list so that Windows binaries can be created. Once those are made, they may
    be found at https://windows.php.net/qa/.

    Here is an example "ready for builds" message to release-managers@php.net:

    ```text
    Subject: PHP 8.1.6RC1 ready for builds

    Hi, all!

    Tag: php-8.1.6RC1
    Tarballs: https://downloads.php.net/~ramsey/
    Manifest: https://gist.github.com/ramsey/5d73f0717effb6d8d17699381361e4b1

    Cheers,
    Ben

    << PASTE FULL MANIFEST CONTENTS HERE >>
    ```


## Announcing a non-stable release (alpha/beta/RC)

1. Switch to your local clone of the `web-qa` repository and update the
   information in the `$QA_RELEASES` array in `include/release-qa.php`.

   Follow the documentation in the file for editing the QA release information.
   See also [Announce 8.1.0RC3][] and [8.1.6RC1][] for examples.

   Add, commit, and push your changes, when finished.

   ```shell
   git add -p
   git commit --gpg-sign=YOURKEYID -m "Announce PHP X.Y.ZRCn"
   git push upstream master
   ```

2. 🔶 **For pre-GA releases only,** add a short notice to `web-php` stating
   there is a new release, and highlight the major changes (e.g., security
   fixes).

   To help produce the files for this, use the `bin/createNewsEntry` tool. When
   you run it, it will ask several questions (see below). For pre-GA non-stable
   releases, use only the "frontpage" category.

   ```shell
   cd /path/to/repos/php/web-php
   ./bin/createNewsEntry
   # Please type in the title: PHP X.Y.0 Alpha n available for testing
   # Categories:
   #     0: PHP.net frontpage news   [frontpage]
   #     1: New PHP release          [releases]
   #     2: Conference announcement  [conferences]
   #     3: Call for Papers          [cfp]
   # Please select appropriate categories, separated with space: 0
   # Will a picture be accompanying this entry? no
   # And at last; paste/write your news item here.
   # To end it, hit <enter>.<enter>
   #
   #   [[ Here you will paste the full HTML of the post. ]]
   # .
   #
   git add -p
   git add archive/entries/*.xml
   git commit --gpg-sign=YOURKEYID -m "Announce PHP X.Y.0RCn"
   git push upstream master
   ```

   Each news entry for pre-GA releases will be similar, though we change the
   text slightly to indicate progression through the pre-release cycle. For
   example, here are all the news posts for the pre-GA releases of PHP 8.1.0:

   * [Announce 8.1.0alpha1](https://github.com/php/web-php/commit/57b9675c8d8550493289fa1fba77427c93cdd472)
   * [Announce 8.1.0alpha2](https://github.com/php/web-php/commit/cec044fc0763f5cfa77d0e79479f8b6279023570)
   * [Announce 8.1.0alpha3](https://github.com/php/web-php/commit/5c480765f444a3fddfd575e01fe0be3fcfdde05b)
   * [Announce 8.1.0beta1](https://github.com/php/web-php/commit/40840e3c3f89d6dd95baa4b8cdf22d6f206f86c2)
   * [Announce 8.1.0beta2](https://github.com/php/web-php/commit/7bf6acdadd4940bd9db711bf3f9d5a4054dc0722)
   * [Announce 8.1.0beta3](https://github.com/php/web-php/commit/38c8a872700fb0c2ebde49e2eae3374257ba6d08)
   * [Announce 8.1.0RC1](https://github.com/php/web-php/commit/6e4bf3d0228ce113728d5f1a769ed42e0d63ca10)
   * [Announce 8.1.0RC2](https://github.com/php/web-php/commit/1ae95f4b686a5d614a94a664a7466ee0e5cd21eb)
   * [Announce 8.1.0RC3](https://github.com/php/web-php/commit/3091246d77a3f445fcc593587597d0abcab8c373)
   * [Announce 8.1.0RC4](https://github.com/php/web-php/commit/fbaeb9403f4e2856115889946d3f63751e183c7b)
   * [Announce 8.1.0RC5](https://github.com/php/web-php/commit/46473ccccfb5f7fedc3f169c55fb7c22a596b55d)
   * [Announce 8.1.0RC6](https://github.com/php/web-php/commit/cacaef9c41352b5dbf3fbbf44702cc6c0cbfb478)

   > ⚠️ **Important** \
   > In your announcement news entry, be sure to include the following text or
   > text similar to the following:
   >
   > > Please DO NOT use this version in production, it is an early test version.

   > 🗒 **Note** \
   > When a version is in its post-GA phase, we do not post news entries for
   > non-stable releases.

3. Wait for the web and qa sites to update with the new information before
   sending announcements. This could take up to an hour.

4. Send *separate* announcement emails to:

   * `internals@lists.php.net`
   * `php-general@lists.php.net`
   * `php-qa@lists.php.net`

   In the announcement message, point out the location of the release and the
   possible release date of either the next RC or the final release. Also
   include the manifest generated by `gen_verify_stub` when you packaged the
   build.

   Here are a few examples of non-stable release announcement emails:

   * [PHP 8.1.0alpha1 is available for testing](https://news-web.php.net/php.qa/69043)
   * [PHP 8.1.0beta3 available for testing](https://news-web.php.net/php.qa/69079)
   * [PHP 8.1.0RC6 available for testing](https://news-web.php.net/php.qa/69117)
   * [PHP 8.1.7RC1 Ready for testing](https://news-web.php.net/php.qa/69163)

   > 🚨 **Send separate emails!** \
   > Do *not* send a single email message with all addresses in the `To`, `Cc`,
   > or `Bcc` headers. If a user replies to one of these messages, we do not
   > want their email client to automatically send the reply to each list, as
   > often occurs.

   > 💬 **Hint** \
   > We send emails to the followers of these mailing lists to notify them of
   > new releases, so they can make sure their projects keep working and can
   > report any potential bugs that should be fixed before the upcoming GA
   > release.

5. 🔶 **For pre-GA *RCs* only,** coordinate with the social media team (i.e.,
   Derick) to post a tweet with the RC release announcement and link to the news
   entry on php.net. ([@official_php](https://twitter.com/official_php))


## Packaging a stable release

1. Check out the *patch-level version branch* for the release
   (e.g., `PHP-8.1.7`).

   > 💬 **Hint** \
   > You should have created this branch when packaging the non-stable release
   > candidate for this version.

2. If a CVE commit needs to be merged to the release, have it committed to
   the base branches and [merged upwards as usual][] (e.g. commit the CVE fix
   to 7.2, merge to 7.3, 7.4, etc.). Then, you can cherry-pick it into the
   patch-level version branch for this release.

   Commit these changes and push the patch-level version branch. Ensure
   that CI is still passing (see above).

   > 💡 **Tip** \
   > Don't forget to update `NEWS` manually in an extra commit to the
   > patch-level version branch.

3. Run the `./scripts/dev/credits` script in the patch-level version branch,
   and commit the changes in the credits files in `ext/standard`.

   > 🗒 **Note** \
   > It's very rare this will make changes at this point, but we run it here
   > in case the credits changed as a result of a bug fix that was
   > cherry-picked into this branch.

4. Create a local-only release branch for this release from the *patch-level
   version branch*.

   ```shell
   git checkout -b php-X.Y.Z-local-release-branch upstream/PHP-X.Y.Z
   ```

5. Using your local-only release branch, bump the version numbers in
   `main/php_version.h`, `Zend/zend.h`, `configure.ac`, and possibly
   `NEWS`.

   For example, if you're releasing a stable version for PHP 8.1.8, then all
   the version numbers in the patch-level version branch should be
   `8.1.8-dev`. In your local-only release branch, you will change them all to
   `8.1.8`.

   See [Update versions for PHP 8.1.7][] and [Update NEWS for PHP 8.1.7][] for
   an example.

6. Compile and run `make test`, with and without ZTS (Zend Thread Safety), using
   the correct Bison and re2c versions, e.g., for PHP 7.4, Bison 3.0.0 and re2c
   0.13.4 are required, as a minimum.

   For example:

   ```shell
   # With ZTS
   make distclean || \
   ./buildconf --force \
       && ./configure --enable-zts --disable-all --enable-debug --enable-opcache --enable-opcache-jit \
       && make -j$(nproc) \
       && make test TEST_PHP_ARGS="-q -j$(nproc)" \
       || ./sapi/cli/php -v

   # Without ZTS
   make distclean || \
   ./buildconf --force \
       && ./configure --disable-all --enable-debug --enable-opcache --enable-opcache-jit \
       && make -j$(nproc) \
       && make test TEST_PHP_ARGS="-q -j$(nproc)" \
       || ./sapi/cli/php -v
   ```

7. After each build, check the output of `./sapi/cli/php -v` to ensure the
   versions match the release.

8. If all is correct, commit the changes to your local-only release branch.

   ```shell
   git add -p
   git commit --gpg-sign=YOURKEYID -m "Update versions for PHP X.Y.Z"
   ```

9. Tag your local-only release branch with the release version and push the tag.

   ```shell
   git tag -s -u YOURKEYID php-X.Y.Z -m "Tag for php-X.Y.Z"
   git push upstream php-X.Y.Z
   ```

10. Run the following using the release tag to export the tree, create the
    `configure` script, and build and compress three tarballs (`.tar.gz`,
    `.tar.bz2` and `.tar.xz`).

    ```shell
    ./scripts/dev/makedist php-X.Y.Z
    ```

    > 💬 **Hint** \
    > Check if the PEAR files are updated (Phar).

    > 💡 **Tip** \
    > On some systems the behavior of GNU tar can default to produce POSIX
    > compliant archives with PAX headers. As not every application is
    > compatible with that format, creation of archives with PAX headers should
    > be avoided. When packaging on such a system, the GNU tar can be influenced
    > by defining the environment variable `TAR_OPTIONS='--format=gnu'`.

11. Run the following using the release tag and your GPG key ID to sign the
    tarballs and save the signatures to `php-X.Y.Z.manifest`, which you can
    upload to GitHub and include in the announcement emails.

    ```shell
    ./scripts/dev/gen_verify_stub X.Y.Z YOURKEYID > php-X.Y.Z.manifest
    ```

12. If you have the [GitHub command line tool][] installed, run the following to
    create a public Gist for the manifest file:

    ```shell
    gh gist create --public php-X.Y.Z.manifest
    ```

    Or you may go to https://gist.github.com to create it manually.

13. Switch to your local clone of the `web-php-distributions` repository and
    copy the tarballs and signature files into the repository. Add, commit, and
    push them.

    ```shell
    cd /path/to/repos/php/web-php-distributions
    mv /path/to/repos/php/php-src/php-X.Y.Z.tar.* .
    git add php-X.Y.Z.tar.*
    git commit --gpg-sign=YOURKEYID -m "Add tarballs for php-X.Y.Z"
    git push upstream master
    ```

14. Switch to your local clone of the `web-php` repository and update the
    `web-php-distributions` submodule.

    ```shell
    cd /path/to/repos/php/web-php
    git pull --rebase upstream master
    git submodule init
    git submodule update
    cd distributions
    git fetch --all
    git pull --rebase upstream master
    cd ..
    git commit distributions -m "X.Y.Z tarballs"
    git push upstream master
    ```

    > 💬 **Hint** \
    > This fetches the last commit ID from `web-php-distributions` and pins the
    > "distributions" submodule in `web-php` to this commit ID.
    >
    > When the website syncs, which should happen within an hour, the tarballs
    > will be available from `https://www.php.net/distributions/php-X.Y.Z.tar.gz`,
    > etc.

15. Once the release is tagged, contact the release-managers@php.net distribution
    list so that Windows binaries can be created. Once those are made, they may
    be found at https://windows.php.net/qa/.

    > ⚠️ **Important** \
    > Do *not* send this announcement to any public lists.

    Here is an example "ready for builds" message to release-managers@php.net:

    ```text
    Subject: PHP 8.1.6 ready for builds

    Hi, all!

    Tag: php-8.1.6
    Tarballs: web-php-distributions
    Manifest: https://gist.github.com/ramsey/432fcf8afcbfb1f1de6c3ab47d82e366

    Cheers,
    Ben

    << PASTE FULL MANIFEST CONTENTS HERE >>
    ```


## Announcing a stable release

1. Switch to your local clone of `web-php` and add the information for the
   previous release to `include/releases.inc`.

   For example, if you are preparing to announce version 8.2.2, then the
   previous release is 8.2.1, so you will add the information for 8.2.1 to this
   file. Most of the time, you can do this using the `bin/bumpRelease` tool.

   ```shell
   ./bin/bumpRelease 8 2
   ```

   The first number is the major version, and the second number is the minor
   version. In this example, we're bumping the release information for version
   8.2. There is no need to provide the patch level.

   > 💡 **Tip** \
   > If this fails for any reason, you can manually copy the information
   > for the previous release from `include/version.inc` into
   > `include/releases.inc`.

2. Update the version information for the new release in `include/version.inc`.

   Find the part of the `$data` array that is related to your version (e.g.,
   `$data['8.2']` for 8.2.x releases), and make the following edits:

   * Set `version` to the full version number (e.g. '8.2.1')
   * Set `date` to the release date in `j M Y` format (e.g. '5 Jan 2021')
   * Update the `tags` array to include `'security'` if this is a security release
   * Update the `sha256` array with the hashes for each of the release tarballs

3. Create the release file and news entry for the new version.

   ```shell
   ./bin/createReleaseEntry -v X.Y.Z -r # --security for security releases
   ```

   This will create a release file (i.e., `releases/X_Y_Z.php`) and a news entry
   file (i.e., `archive/entries/YYYY-MM-DD-n.xml`), while also updating
   `archive/archive.xml`.

   Within these files, it will generate standard messages for the new version.
   You may edit the generated files to expand on the base message, if needed.

4. Update the ChangeLog file for the given major version (e.g., `ChangeLog-8.php`).

   ```shell
   ./bin/news2html 'https://github.com/php/php-src/raw/php-X.Y.Z/NEWS' 'X.Y.Z' 'ChangeLog-X.php'
   ```

5. Review all the changes in `web-php`, commit, and push them.

   ```shell
   git add -p
   git add archive/entries/*.xml releases/*.php
   git commit --gpg-sign=YOURKEYID -m "Announce PHP X.Y.Z"
   git push upstream master
   ```

   See [Announce PHP 8.1.6][] for an example commit.

6. Switch to your local clone of the `web-qa` repository and update the
   information in the `$QA_RELEASES` array in `include/release-qa.php`.

   The array probably contains information about the RC released two weeks ago
   in preparation for the current release. Since the current release is now GA,
   it's time to remove the RC build from the QA website.

   It is sufficient to set the `number` property for the release to `0` to
   stop displaying the RC build on the QA website. You may also remove the
   sha256 hashes for the RC tarballs, but it's not necessary. For an example,
   see [PHP 8.1.6 released][].

   Add, commit, and push your changes, when finished.

   ```shell
   git add -p
   git commit --gpg-sign=YOURKEYID -m "PHP X.Y.Z released"
   git push upstream master
   ```

7. 🚨 **Before sending announcement emails, check to make sure the websites have
   synced.**

   * Make sure the tarballs are available from, e.g.,
     `https://www.php.net/distributions/php-X.Y.Z.tar.gz`
   * Check the "downloads" page to make sure the new version appears:
     https://www.php.net/downloads
   * Does the news entry show up on the home page? https://www.php.net
   * Do the updates to the ChangeLog appear?
     e.g., https://www.php.net/ChangeLog-8.php
   * Is there a release page for the new version?
     e.g., `https://www.php.net/releases/X_Y_Z.php`
   * Does the RC for this version still appear on the QA home page?
     https://qa.php.net

   Keep in mind it may take up to an hour for the websites to sync.

8. Please note down the sha256 and the PGP signature (.asc). These *must* be
   included in the release mail.

9. Send *separate* announcement emails to:

   * `php-announce@lists.php.net`
   * `php-general@lists.php.net`
   * `internals@lists.php.net`

   Release announcement emails must include the manifest generated when
   packaging the build, along with links to the sources, Windows binaries, and
   changelog. Here are a few examples of stable release announcement emails:

   * [PHP 8.1.0 Released](https://news-web.php.net/php.announce/321)
   * [PHP 8.1.3 Released](https://news-web.php.net/php.announce/325)
   * [PHP 8.1.6 Released](https://news-web.php.net/php.announce/331)

   > ⚠️ **Important** \
   > For standard patch-level releases, we will note "This is a bugfix release."
   > If it is a security release, we must note "This is a security release."

   > 🚨 **Send separate emails!** \
   > Do *not* send a single email message with all addresses in the `To`, `Cc`,
   > or `Bcc` headers. If a user replies to one of these messages, we do not
   > want their email client to automatically send the reply to each list, as
   > often occurs.

10. Coordinate with the social media team (i.e., Derick) to post a tweet with
    the release announcement and link to the news entry on php.net.
    ([@official_php](https://twitter.com/official_php))


## Re-releasing the same version or a patch-level (i.e., `-plN`)

While unlikely, there may be times we need to re-release the same version. This
might happen if the tarballs have a corrupted file, for example.

Should this occur *before* announcing the release, you may choose to delete the
tag and go through the full packaging process again, as described above.

> 💬 **Hint** \
> This is one of the reasons we package releases two days before announcing
> them.

If this happens *after* announcing the release, you may choose to tag, package,
and release a patch-level (i.e., *pl*) release. If it is not critical and/or
affects a very limited subset of users, then you may choose to wait until the
next release.

If you choose to create a patch-level release, follow these steps:

1. Commit the new binaries to `web-php-distributions`

2. Update $data['X.Y'] in `web-php:/include/version.inc`
   (X.Y=major.minor release, e.g. '8.0'):

    * `version` to the full version number (e.g. '8.0.1-pl1')
    * `date` to the release date in `j M Y` format (e.g. '9 Jan 2021')
    * `tags` array should include `security` if this is a security release
    * `sha256` array and sub-elements for all SHA256 sums

3. Add a short notice to `web-php` stating that there is a new release, and
   highlight the major important things (security fixes) and when it is
   important to upgrade.

    * Call `php bin/createReleaseEntry -v <version> [ --security ]` in your
      local web-php checkout.

4. Commit all the changes (`include/version.inc`, `archive/archive.xml`,
   `archive/entries/YYYY-MM-DD-N.xml`).

5. Wait an hour or two, then send a mail to php-announce@lists.php.net,
   php-general@lists.php.net and internals@lists.php.net with a text similar to
   the news entry.

   Please make sure that the mail to php-announce@ is its own completely
   separate email. This is to make sure that replies to the announcement on
   php-general@ or internals@ will not accidentally hit the php-announce@
   mailinglist.


## Feature freeze

A major/minor version [feature freeze][] occurs with the first beta release.
Specifically, it occurs when the first beta release is packaged, which means the
feature freeze occurs two days before the first beta release.

The feature freeze for `php-src` means that we will not accept any new features
after the date of the feature freeze. For any RFCs to be included in the new
version, they should be discussed and have the voting polls closed no later than
the feature freeze date. However, this does not mean the new feature must have a
complete implementation by this date.

Following the feature freeze, the focus of work for the new version will be on
fixing bugs, writing tests, and completing/polishing all accepted features.

As a courtesy to the community, the release managers should remind others about
the upcoming feature freeze by posting reminders to internals@lists.php.net at
4-weeks, 3-weeks, 2-weeks, and 1-week prior to the feature freeze. This is a
recommendation and the intervals may vary based on work load.


## Forking a new version branch

When the new version has reached the first RC, it is time to create a new
version branch. This frees up the main branch (i.e., `master`) for any new
feature development that cannot go into the new version.

1. One week prior to tagging `X.Y.0RC1`, warn internals@ that your version's
   branch is about to be created. Be specific about when the branch creation
   will occur. For example: https://news-web.php.net/php.internals/99864

2. Just prior to tagging `X.Y.0RC1`, create the new version branch locally,
   i.e. `PHP-X.Y`.

3. Add a commit on the main branch (i.e., `master`) after the branch point.

   This commit should:

   * clear the `NEWS`, `UPGRADING`, and `UPGRADING.INTERNALS` files;
   * update the version numbers in `configure.ac`, `main/php_version.h`,
     `Zend/zend.h`, and `win32/build/confutils.js`;
   * update the API version numbers in `Zend/zend_extensions.h`,
     `Zend/zend_modules.h`, and `main/php.h`; and
   * add the new branch to the list in `CONTRIBUTING.md`.

   See [Prepare for PHP 8.2][] and [Prepare for PHP 8.2 (bis)][] for an example
   of what this commit should include.

4. Push the new version branch and the changes to the `master` branch, with an
   appropriate commit message (e.g., "master is now for PHP 8.3.0-dev").

5. Immediately notify internals@ of the new branch and advise on the new merging
   order. For example: https://news-web.php.net/php.internals/99903

6. Update `web-php:git.php` and https://wiki.php.net/vcs/gitworkflow to reflect
   the new branch.

   For example:

   * [Add PHP-8.1 to the Git steps page][]
   * [Changes to the wiki][]

> 💬 **Hint** \
> We create the new version branch at the first release candidate rather than at
> feature freeze to allow a period of time where the focus is on making the new
> version ready for RC and GA. During this time, the main branch is *only* for
> minor improvements and bug fixes. All major improvements and new features must
> wait.


## Preparing for the initial stable version (PHP X.Y.0)

1. When you release the first pre-GA RC, remind the documentation team
   (phpdoc@lists.php.net) to write the [migration guide][]. Make sure the guide
   is available before releasing the initial stable version, since you should
   link to it in the release announcements.

2. Timely get used to the differences in preparing and announcing a stable
   release.

3. Before releasing X.Y.0, merge the `NEWS` entries of the pre-releases, so that
   there is only a single section about PHP X.Y.0, instead of individual
   sections for each pre-release.

4. On the announcement day for the initial stable version (or shortly before),
   update the `Expires` field in the <https://www.php.net/.well-known/security.txt>.
   file. The `Expires` field should be set to the expected date of the next X.Y.0
   release (following the one currently being prepared), which is usually the
   fourth Thursday of November in the next year.

   Following the recommendation of [RFC 9116](https://www.rfc-editor.org/rfc/rfc9116),
   we maintain an `Expires` time of about a year for our security policies. This
   provides security researchers with confidence they are using our most
   up-to-date reporting policies.

   The `security.txt` file is located in the [web-php repository](https://github.com/php/web-php)
   under the `.well-known/` directory. We may make changes to this file at other
   times, as needed, but we will always advance the `Expires` timestamp on a
   yearly cadence, coinciding with our X.Y.0 releases.

   Please see the instructions in the comments within the `security.txt` file
   for information on how to update it.


## Prime the selection of release managers for the next version

About three months prior to the scheduled release of the first alpha release of
the next  minor or major version (around March 1st or shortly thereafter), the
release managers for the latest version branch should issue a call for
volunteers to begin the selection process for the next release managers.

1. Issue the call for volunteers on internals@lists.php.net on or around March
   1st. See, for example: https://news-web.php.net/php.internals/113334

   There is no rule for how long the call for volunteers must remain open. We
   should aim to select the release managers by early April, so announcing the
   call in early March gives people about a month to decide whether they wish to
   volunteer.

2. There should be two or more volunteers. Typically, one should be a veteran
   release manager (having previously served as a `php-src` release manager),
   while the other one (or two) should be rookies. Hold a vote if necessary (see
   https://wiki.php.net/rfc/releaseprocess#release_managers_selection).

3. Help the new release managers with their first steps.


## New release manager checklist

1. Email systems@php.net to get setup for access to downloads.php.net and to be
   added to the release-managers@php.net distribution list.

2. Request membership to the release managers group on GitHub.

3. Create a [GPG key][] for your @php.net address.

   > 💡 **Tip** \
   > If you're new to GPG, follow GitHub's instructions for
   > [Generating a new GPG key][].

   Publish your key by editing `include/gpg-keys.inc` in the `web-php`
   repository. Add a `case` for your username to the `gpg_key_get()` function,
   and paste the output from `gpg --fingerprint`. You may also need to update
   the `$branches` array in the `gpg_key_get_branches()` function to include
   your username alongside your branch.

   ```console
   ❯ gpg --fingerprint ramsey@php.net
   pub   rsa4096 2021-04-26 [SC] [expires: 2025-11-24]
   39B6 4134 3D8C 104B 2B14  6DC3 F9C3 9DC0 B969 8544
   uid           [ultimate] Ben Ramsey <ramsey@php.net>
   sub   rsa4096 2021-04-26 [E] [expires: 2025-11-24]
   ```

   Have one or more of the other RMs [sign your GPG key][], and publish your
   public key to a keyserver:

   ```shell
   gpg --keyserver keys.openpgp.org --send-keys YOURKEYID
   gpg --keyserver keyserver.ubuntu.com --send-keys YOURKEYID
   ```

   Add your public key to `php-keyring.gpg` in `web-php-distributions`. To do
   this, you will need to import all keys from the current PHP keyring file to
   your local GPG keyring. You will need to take note of the key IDs for each of
   the release managers listed in `php-keyring.gpg`. Then, you will export,
   specifying your key ID in addition to the key IDs of every other release
   manager. Save this export back to `php-keyring.gpg`, commit the changes,
   and push.

   ```shell
   cd /path/to/repos/php/web-php-distributions
   gpg php-keyring.gpg            # lists all keys in the keyring
   gpg --import php-keyring.gpg   # imports all keys to your local keyring
   gpg --export \
       --export-options export-minimal \
       --armor \
       YOURKEYID F9C39DC0B9698544 DBDB397470D12172 MORE RM KEY IDS ... \
       > php-keyring.gpg
   gpg php-keyring.gpg  # verify all the keys are present, including yours
   git add -p
   git commit --gpg-sign=YOURKEYID -m "Update PHP release manager keyring"
   git push
   ```

   `web-php-distributions` is a submodule of `web-php`. You'll now have to update
   the commit reference to reflect the change made in web-php-distributions.

   ```shell
   cd /path/to/repos/php/web-php
   git submodule update
   cd distributions           # This is the submodule referring to web-php-distributions
   git pull origin master
   cd ..
   git add distributions
   git commit --gpg-sign=YOURKEYID -m "Update php-keyring.gpg in distributions"
   git push
   ```

4. Request moderation access to php-announce@lists.php.net
   so you are able to moderate your release announcements. All the announcements
   should be sent from your @php.net address.

   > 💬 **Hint** \
   > To send email from your @php.net address, you will need to use a custom
   > SMTP server. If you use Gmail, you may
   > "[Send emails from a different address or alias][]."

5. Make sure you have the following repositories cloned locally:

   * https://github.com/php/php-src
   * https://github.com/php/web-php
   * https://github.com/php/web-qa
   * https://github.com/php/web-php-distributions


[general availability]: https://en.wikipedia.org/wiki/Software_release_life_cycle#General_availability_(GA)
[muscle memory]: https://en.wikipedia.org/wiki/Muscle_memory
[Conditional Includes For Git Config]: https://motowilliams.com/2017-05-11-conditional-includes-for-git-config/
[Update versions for PHP 8.1.0beta3]: https://github.com/php/php-src/commit/3edd1087c70bee2ec21f0fbec1a575d78a500f15
[Update versions for PHP 8.1.6RC1]: https://github.com/php/php-src/commit/40e8ced23898e3069340ca03ea5febc5361015ad
[Update NEWS for PHP 8.1.6RC1]: https://github.com/php/php-src/commit/a4fdeaebe419b88e3b4a1f5aba845c2d4e81fd4e
[Prepare for PHP 8.1.0RC1]: https://github.com/php/php-src/commit/5764414eb8900ae98020a3c20693f4fb793efa99
[Bump for 8.1.8-dev]: https://github.com/php/php-src/commit/3b6ee1eb19c14c3339ebfcf5c967065a9f828971
[GitHub command line tool]: https://cli.github.com
[Announce 8.1.0RC3]: https://github.com/php/web-qa/commit/f264b711fd3827803b79bbb342959eae57ea502b
[8.1.6RC1]: https://github.com/php/web-qa/commit/e6d61ad7a9d8be0b1cd159af29f3b9cbdde33384
[merged upwards as usual]: https://wiki.php.net/vcs/gitworkflow
[Update versions for PHP 8.1.7]: https://github.com/php/php-src/commit/d35e577a1bd0b35b9386cea97cddc73fd98eed6d
[Update NEWS for PHP 8.1.7]: https://github.com/php/php-src/commit/b241f07f52ca9f87bf52be81817f475e6e727439
[Announce PHP 8.1.6]: https://github.com/php/web-php/commit/9f796a96c65f07e45845ec248933bfb0010b94a9
[PHP 8.1.6 released]: https://github.com/php/web-qa/commit/bff725f8373cf6fd9d97ba62a8517b19721a4c2e
[feature freeze]: https://en.wikipedia.org/wiki/Freeze_(software_engineering)
[Prepare for PHP 8.2]: https://github.com/php/php-src/commit/1c33ddb5e5598c5385c4c965992c6e031fd00dd6
[Prepare for PHP 8.2 (bis)]: https://github.com/php/php-src/commit/a93e12f8a6dfc23e334339317c97aa35356db821
[Add PHP-8.1 to the Git steps page]: https://github.com/php/web-php/commit/1fcd78c2817cf1fbf1a1de2ddec1350be4e26491
[Changes to the wiki]: https://wiki.php.net/vcs/gitworkflow?do=diff&rev2%5B0%5D=1617123194&rev2%5B1%5D=1654728193&difftype=sidebyside
[migration guide]: https://www.php.net/manual/en/migration81.php
[GPG key]: https://en.wikipedia.org/wiki/GNU_Privacy_Guard
[Generating a new GPG key]: https://docs.github.com/en/authentication/managing-commit-signature-verification/generating-a-new-gpg-key
[sign your GPG key]: https://carouth.com/articles/signing-pgp-keys/
[Send emails from a different address or alias]: https://support.google.com/mail/answer/22370?hl=en
