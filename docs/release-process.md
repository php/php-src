# PHP release process

A release managers role includes making packaged source code from the canonical repository
available according to their release schedule.

The release schedule for their branch is available on a Wiki page, for example
  [PHP 8.0 release schedule](https://wiki.php.net/todo/php80)

There are two types of release:

  - non stable (alpha/beta/RC)
  - stable
  
The process of making packaged source available and announcing availability is explained
in detail below. The process differs slightly for non-stable and stable releases.

Before a release manager can begin their work, there are several things that must happen,
explained at the end of this document in 
	[New Release Manager Checklist](#new-release-manager-checklist).

## General notes and tips

 1. Do not release on Fridays, Saturdays, or Sundays as this gives poor lead time for 
    downstream consumers adhering to a typical work week.

 2. Package two days before a release. So if the release is to be on Thursday,
    package on Tuesday. Think about timezones as well.

 3. Ensure that the relevant tests on CI are green.

    See:
      https://ci.appveyor.com/project/php/php-src
      https://dev.azure.com/phpazuredevops/PHP/

    It is recommended to do so a couple of days before the packaging day, to
    have enough time to investigate failures, communicate with the authors and
    commit the fixes.

    Check the CI status for your branch periodically and resolve the failures ASAP. 
    
    See more in https://wiki.php.net/rfc/travis_ci.

 4. Follow all steps to the letter. When unclear ask previous RM's before
    proceeding. Ideally make sure that for the first releases one of the
    previous RM's is around to answer questions. For the steps related to the
    php/QA/bug websites try to have someone from the webmaster team on hand.

 5. Verify the tags to be extra sure everything was tagged properly.

 6. There is a PHP release Docker image https://github.com/sgolemon/php-release
    with forks available to help releasing.

 7. Communication with previous release managers should be conducted via
    release-managers@php.net
  
 8. References to repositories in this document refer to the canonical source 
    located at https://github.com/php

## Packaging a non stable release (alpha/beta/RC)

 1. Check the tests at https://travis-ci.org/php/php-src/builds.

 2. Run the `scripts/dev/credits` script in php-src and commit the changes in
    the credits files in ext/standard.

 3. Checkout the release branch for this release (e.g., PHP-7.4.2) from the main
    branch.

 4. Bump the version numbers in `main/php_version.h`, `Zend/zend.h`,
    `configure.ac` and possibly `NEWS`. Do not use abbreviations for alpha and
    beta. Do not use dashes, you should `#define PHP_VERSION "7.4.22RC1"` and
    not `#define PHP_VERSION "7.4.22-RC1"`.

    When releasing the first release candidate, you must also bump the API
    version numbers in `Zend/zend_extensions.h`, `Zend/zend_modules.h`, and
    `main/php.h`. The API versions between the alpha/beta/.0RCx releases can be
    left the same, or bumped as little as possible because PHP extensions will
    need to be rebuilt with each bump. Do *not* bump the API versions after
    RC1.

 5. Compile and run `make test`, with and without ZTS, using the right Bison and
    re2c version (for PHP 7.4, minimum Bison 3.0.0 and re2c 0.13.4 are used).

 6. Check `./sapi/cli/php -v` output for version matching.

 7. If all is right, commit the changes to the release branch:

    ```sh
    git commit -a
    ```

 8. Tag the repository release branch with the version, e.g.:

    ```sh
    git tag -u YOURKEYID php-7.4.2RC2
    ```

 9. Bump the version numbers in `main/php_version.h`, `Zend/zend.h`,
    `configure.ac` and `NEWS` in the *main* branch (PHP-7.4 for example) to
    prepare for the **next** version. For example, if the RC is "7.4.1RC1" then
    the new one should be `7.4.2-dev` - regardless if we get a new RC or not.
    This is to make sure `version_compare()` can correctly work. Commit the
    changes to the main branch.

10. Push the changes to the `php-src`. 

    Non stable release branches don't need to be pushed 
      (a local temporary branch should be used).

    ```sh
    git push --tags origin HEAD
    git push origin {main branch}
    git push origin {release branch}
    ```

11. Run: `./scripts/dev/makedist php-7.4.0RC2`, this will export the tree,
    create `configure` and build three tarballs (gz, bz2 and xz).

12. Run `./scripts/dev/gen_verify_stub <version> [identity]`, this will sign the
    tarballs and output verification information to be included in announcement
    email.

13. Copy those tarballs (scp, rsync) to downloads.php.net, in your homedir there
    should be a directory `public_html/`. Copy them into there. If you do not
    have this directory, create it.

14. Now the RC can be found on https://downloads.php.net/~yourname,
    e.g. https://downloads.php.net/~derick/.

15. Once the release has been tagged, contact the release-managers@ distribution
    list so that Windows binaries can be created. Once those are made, they can
    be found at https://windows.php.net/download.

## Announcing a non stable release (alpha/beta/RC)

 1. Update `web-qa:include/release-qa.php` with the appropriate information. See
    the documentation within release-qa.php for more information, but all
    releases and RCs are configured here. Only `$QA_RELEASES` needs to be
    edited.

    Example: When packaging an RC, set the `rc` with appropriate information for
    the given version.

    Note: Remember to update the sha256 checksum information.

 2. Skip this step for non stable releases after GA of minor or major versions
    (e.g. announce 7.4.0RC1, but not 7.4.1RC1):

    Add a short notice to web-php stating that there is a new release, and
    highlight the major important things (security fixes) and when it is
    important to upgrade.

    * Call `php bin/createNewsEntry` in your local web-php checkout. Use category
      "frontpage" *and* "releases" for all stable releases. Use category
      "frontpage" for X.Y.0 non-stable releases only (news only).

    * Add the content for the news entry. Be sure to include the text:

        ```text
        "THIS IS A DEVELOPMENT PREVIEW - DO NOT USE IT IN PRODUCTION!"
        ```

 3. Commit and push changes to qa and web.

    Wait for web and qa sites to update with new information before sending
    announce.

 4. Send **separate** emails **To** `internals@lists.php.net` and
    `php-general@lists.php.net` lists pointing out "the location of the release"
    and "the possible release date of either the next RC, or the final release".
    Include in this information the verification information output by
    `gen_verify_stub`.

 5. Send **separate** emails (see example http://news.php.net/php.pear.qa/5201)
    **To** `php-qa@lists.php.net` and `primary-qa-tester@lists.php.net`. These
    emails are to notify the selected projects about a new release so that they
    can make sure their projects keep working. Make sure that you have been
    setup as a moderator for `primary-qa-tester@lists.php.net` by having someone
    (Hannes, Dan, Derick) run the following commands for you:

    ```sh
    ssh lists.php.net
    sudo -u ezmlm ezmlm-sub ~ezmlm/primary-qa-tester/mod moderator-email-address
    ```
 6. For RCs, post tweet with release announcement (and link to news article on
    php.net) ([@official_php](https://twitter.com/official_php))

## Packaging a stable release

 1. Checkout your release branch, you should have created when releasing
    previous RC and bump the version numbers in `main/php_version.h`,
    `Zend/zend.h`, `configure.ac` and possibly `NEWS`.

 2. If a CVE commit needs to be merged to the release, then have it committed to
    the base branches and merged upwards as usual (e.g. commit the CVE fix to
    7.2, merge to 7.3, 7.4 etc...). Then you can cherry-pick it in your release
    branch. Don't forget to update `NEWS` manually in an extra commit then.

 3. Commit those changes. Ensure the tests at
    https://travis-ci.org/php/php-src/builds are still passing.

 4. Run the `scripts/dev/credits` script in php-src and commit the changes in
    the credits files in ext/standard.

 5. Compile and run `make test`, with and without ZTS, using the right Bison and
    re2c version (for PHP 7.4, minimum Bison 3.0.0 and re2c 0.13.4 are used).

 6. Check `./sapi/cli/php -v` output for version matching.

 7. Tag the repository with the version e.g. `git tag -u YOURKEYID php-7.4.1`

 8. Push the tag e.g. `git push origin php-7.4.1`.

 9. Run: `./scripts/dev/makedist php-7.4.1`, this will export the tag,
    create configure and build three tarballs (gz, bz2 and xz). Check if the
    pear files are updated (phar). On some systems the behavior of GNU tar can
    default to produce POSIX compliant archives with PAX headers. As not every
    application is compatible with that format, creation of archives with PAX
    headers should be avoided. When packaging on such a system, the GNU tar can
    be influenced by defining the environment variable
    `TAR_OPTIONS='--format=gnu'`.

10. Run `scripts/dev/gen_verify_stub <version> [identity]`, this will sign the
    tarballs and output verification information to be included in announcement
    email.

11. Commit and push all the tarballs and signature files to
    `web-php-distributions`, then update the git submodule reference in
    `web-php`:

    ```sh
    git submodule init
    git submodule update
    cd distributions
    git fetch
    git pull --rebase origin master
    cd ..
    git commit distributions
    git push
    ```

    This is to fetch the last commit id from `web-php-distributions` and commit
    this last commit id to `web-php`, then, website will now sync.

12. Once the release has been tagged, contact release managers, Windows
    builders, and package maintainers so that they can build releases. Do not
    send this announcement to any public lists.

## Announcing a stable release

 1. Update `web-php:include/releases.inc` with the old release info (updates the
    download archives).

    * You can run `php bin/bumpRelease 7 4` where the first number is the major
      version, and the second number is the minor version (7.4 in this example).

    * If that fails for any non-trivially fixable reason, you can manually copy
      the old information to `include/releases.inc`.

 2. Update $data['X.Y'] in `web-php:include/version.inc`
    (X.Y=major.minor release, e.g. '8.0'):

    * `version` to the full version number (e.g. '8.0.1')
    * `date` to the release date in `j M Y` format (e.g. '5 Jan 2021')
    * `tags` array should include `security` if this is a security release
    * `sha256` array and sub-elements for all SHA256 sums

 3. Create the release file (`releases/x_y_z.php`):

    Optionally use `bin/createReleaseEntry -v x.y.z -r` to create
    a standard announcement template for this and step 6.
    Add `--security` for a security release.

    Usually we use the same content as for point 6, but included in php
    template instead of the release xml.

    Edit the generated files to expand on the base message if needed.

 4. Update `web-qa:include/release-qa.php` and add the next version as an
    QARELEASE (prepare for next RC). Keep `active => true` until there will be
    no more QA releases. Setting the release number to 0 is sufficient to
    remove the old QA release from the available QA releases list.

 5. Update the ChangeLog file for the given major version

    e.g. `ChangeLog-7.php` from the `NEWS` file

    * You may want to try `web-php:bin/news2html` to automate this task.

    * Go over the list and put every element on one line.
    * Check for `&`, `<` and `>` and escape them if necessary.
    * Remove all the names at the ends of lines.
    * For marking up, you can do the following (with `vi`):

        I. `s/^- /<li>/`

        II. `s/$/<\/li>/`

        III. `s/Fixed bug #\([0-9]\+\)/<?php bugfix(\1); ?>/`

        IV. `s/Fixed PECL bug #\([0-9]\+\)/<?php peclbugfix(\1); ?>/`

        V. `s/FR #\([0-9]\+\)/FR <?php bugl(\1); ?>/`

 6. Add a short notice to `web-php` stating that there is a new release, and
    highlight the major important things (security fixes) and when it is
    important to upgrade.

    * Call `php bin/createReleaseEntry -v <version> [ --security ]` in your
      local web-php checkout.

 7. Commit and push all the changes to their respective git repos

 8. **Check website has been synced before announcing or pushing news**

    Try, e.g. https://www.php.net/distributions/php-7.4.0.tar.xz

    Website may update slowly (may take an hour).

 9. Please note down the sha256 and the PGP signature (.asc). These *must* be
    included in the release mail.

10. Wait an hour or two, then send a mail to php-announce@lists.php.net,
    php-general@lists.php.net and internals@lists.php.net with a text similar to
    http://news.php.net/php.internals/17222. Please make sure that the mail to
    php-announce@ is its own completely separate email. This is to make sure
    that replies to the announcement on php-general@ or internals@ will not
    accidentally hit the php-announce@ mailinglist.

11. Post tweet with release announcement and link to news article on php.net
    ([@official_php](https://twitter.com/official_php))

## Re-releasing the same version (or -pl)

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

## Forking a new release branch

 1. One week prior to cutting X.Y.0beta1, warn internals@ that your version's
    branch is about to be cut, and that PHP-X.Y will be moving into feature
    freeze. Try to be specific about when the branch will be cut.

    Example: http://news.php.net/php.internals/99864

 2. Just prior to cutting X.Y.0beta1, create the new branch locally.

    Add a commit on master after the branch point clearing the `NEWS`,
    `UPGRADING` and `UPGRADING.INTERNALS` files, updating the version in
    `configure.ac` (run `./configure` to automatically update
    `main/php_versions.h`, too) and `Zend/zend.h`. Bump the default initial
    version also in `win32/build/confutils.js`.

    Also list the new branch in `CONTRIBUTING.md`.

    Bump API version numbers in `Zend/zend_extensions.h`, `Zend/zend_modules.h`,
    and `main/php.h`.

    Example:
    https://github.com/php/php-src/commit/a63c99ba624cff86292ffde97089428e68c6fc10

    Push the new branch and the commit just added to master.

 3. Immediately notify internals@ of the branch cut and advise the new merging
    order. Example:

    http://news.php.net/php.internals/99903

 4. Update `web-php:git.php` and https://wiki.php.net/vcs/gitworkflow to reflect
    the new branch. Example:

    https://github.com/php/web-php/commit/74bcad4c770d95f21b7fbeeedbd76d943bb83f23

 5. Notify nlopess@ to add PHP_X_Y tag to gcov.php.net.

## Preparing for the initial stable version (PHP X.Y.0)

 1. About the time you release the first RC, remind the documentation team
    (phpdoc@lists.php.net) to write the migration guide. See to it that they
    have done it before you release the initial stable version, since you want
    to link to it in the release announcements.

 2. Timely get used to the differences in preparing and announcing a stable
    release.

 3. Before releasing X.Y.0, merge the NEWS entries of the pre-releases, so that
    there is only a single section about PHP X.Y.0, instead of individual
    sections for each pre-release.

## Prime the selection of the Release Managers of the next version

This should be done by one of the release managers of the latest release
branch:

 1. About three months before the scheduled release of the first alpha of the
    next minor or major release, issue a call for volunteers on
    internals@lists.php.net (cf. http://news.php.net/php.internals/98652).

 2. Make sure that there are two or more volunteers, and hold a vote if
    necessary (see
    https://wiki.php.net/rfc/releaseprocess#release_managers_selection).

 3. Help the new release managers with their first steps.

## New Release Manager Checklist

 1. Email systems@php.net to get setup for access to downloads.php.net and to be added
    to the release-managers@ distribution list.
    
 2. Request membership to the Release Managers group on github.

 3. Create a GPG key for your @php.net address and publish it by editing
    `include/gpg-keys.inc` in the `web-php` repository, adding the output of
    `gpg --fingerprint "$USER@php.net"`. Let one or more of the previous RMs
    sign your key. Publish your public key to pgp.mit.edu with:
    `gpg --keyserver pgp.mit.edu --send-keys $KEYID`
    Add new keys in the php-keyring.gpg in distribution repository using:
    `gpg2 --export --export-options export-minimal --armor <all RM keys>`

 4. Request moderation access to php-announce@lists.php.net and
    primary-qa-tester@lists.php.net lists, to be able to moderate your release
    announcements. All the announcements should ideally be sent from the
    @php.net alias. Note, that for sending emails as @php.net alias a custom
    SMTP server needs to be used.
