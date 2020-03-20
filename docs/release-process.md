# PHP release process

## General notes and tips

 1. Do not release on Fridays, Saturdays or Sundays because the sysadmins cannot
    upgrade stuff then.

 2. Package two days before a release. So if the release is to be on Thursday,
    package on Tuesday. Think about timezones as well.

 3. Ensure that the tests on Travis CI are green.

    See: https://travis-ci.org/php/php-src/builds

    It is recommended to do so a couple of days before the packaging day, to
    have enough time to investigate failures, communicate with the authors and
    commit the fixes.

    The RM for the branch is also responsible for keeping the CI green on
    ongoing basis between the releases. Check the CI status for your branch
    periodically and resolve the failures ASAP. See more in
    https://wiki.php.net/rfc/travis_ci.

 4. Ensure that Windows builds will work before packaging.

 5. Follow all steps to the letter. When unclear ask previous RM's (David,
    Julien, Johannes, Stas, Derick, Ilia, Sara, Remi, or Christoph) before
    proceeding. Ideally make sure that for the first releases one of the
    previous RM's is around to answer questions. For the steps related to the
    php/QA/bug websites try to have someone from the webmaster team (Bjori) on
    hand.

 6. Verify the tags to be extra sure everything was tagged properly.

 7. Moving extensions from/to PECL requires write access to the destination.
    Most developers should have this.

    Moving extensions from php-src to PECL:

    * Ask someone with Git admin access to create a new empty repository on
      https://git.php.net under the PECL projects and a belonging GitHub mirror.

    * Clone a new copy of the php-src repository (it will rewrite history, keep
      contributors commits and include only the extension folder):

        ```sh
        git clone https://git.php.net/repository/php-src.git ext-name
        cd ext-name
        git filter-branch --prune-empty --subdirectory-filter ext/ext-name master
        ```

    * Set remote Git push URL for the PECL extension:

        ```sh
        git remote set-url origin git@git.php.net:pecl/category/ext-name
        ```

    * Create branch and tags structure appropriate for the extension and push:

        ```sh
        git push -u origin master
        ```

    If the extension is still usable or not dead, in cooperation with the
    extension maintainers if any:

    * Create the pecl.php.net/foo package and its content, license, maintainer
    * Create the package.xml, commit
    * Release the package

    For moving extensions from PECL to php-src the procedure needs to go through
    the RFC process and a new Git commit without rewriting the php-src Git
    commit history.

 8. There is a PHP release Docker image https://github.com/sgolemon/php-release
    with forks available to help releasing.

## Rolling a non stable release (alpha/beta/RC)

 1. Check Windows snapshot builder logs https://windows.php.net/downloads/snaps/
    the last revision.

 2. Check the tests at https://travis-ci.org/php/php-src/builds.

 3. Run the `scripts/dev/credits` script in php-src and commit the changes in
    the credits files in ext/standard.

 4. Checkout the release branch for this release (e.g., PHP-7.4.2) from the main
    branch.

 5. Bump the version numbers in `main/php_version.h`, `Zend/zend.h`,
    `configure.ac` and possibly `NEWS`. Do not use abbreviations for alpha and
    beta. Do not use dashes, you should `#define PHP_VERSION "7.4.22RC1"` and
    not `#define PHP_VERSION "7.4.22-RC1"`.

    When releasing the first alpha version, bump also API version numbers in
    `Zend/zend_extensions.h`, `Zend/zend_modules.h`, and `main/php.h`. The API
    versions between the alpha/beta/.0RCx releases can be left the same or
    bumped as little as possible because PHP extensions will need to be rebuilt
    with each bump.

 6. Compile and run `make test`, with and without ZTS, using the right Bison and
    re2c version (for PHP 7.4, minimum Bison 3.0.0 and re2c 0.13.4 are used).

 7. Check `./sapi/cli/php -v` output for version matching.

 8. If all is right, commit the changes to the release branch:

    ```sh
    git commit -a
    ```

 9. Tag the repository release branch with the version, e.g.:

    ```sh
    git tag -u YOURKEYID php-7.4.2RC2
    ```

10. Bump the version numbers in `main/php_version.h`, `Zend/zend.h`,
    `configure.ac` and `NEWS` in the *main* branch (PHP-7.4 for example) to
    prepare for the **next** version. For example, if the RC is "7.4.1RC1" then
    the new one should be `7.4.2-dev` - regardless if we get a new RC or not.
    This is to make sure `version_compare()` can correctly work. Commit the
    changes to the main branch.

11. Push the changes to the main repo, the tag, the main branch and the release
    branch. Release branches for alpha/beta/.0RCx releases before the GA release
    don't need to be pushed (a local temporary branch should be used).

    ```sh
    git push --tags origin HEAD
    git push origin {main branch}
    git push origin {release branch}
    ```

12. Run: `./scripts/dev/makedist php-7.4.0RC2`, this will export the tree,
    create `configure` and build three tarballs (gz, bz2 and xz).

13. Run `scripts/dev/gen_verify_stub <version> [identity]`, this will sign the
    tarballs and output verification information to be included in announcement
    email.

14. Copy those tarballs (scp, rsync) to downloads.php.net, in your homedir there
    should be a directory `public_html/`. Copy them into there. If you do not
    have this directory, create it.

15. Now the RC can be found on https://downloads.php.net/~yourname,
    e.g. https://downloads.php.net/~derick/.

16. Once the release has been tagged, contact the release-managers@ distribution
    list so that Windows binaries can be created. Once those are made, they can
    be found at https://windows.php.net/download.

## Getting the non stable release (alpha/beta/RC) announced

 1. Update `qa.git/include/release-qa.php` with the appropriate information. See
    the documentation within release-qa.php for more information, but all
    releases and RCs are configured here. Only `$QA_RELEASES` needs to be
    edited.

    Example: When rolling an RC, set the `rc` with appropriate information for
    the given version.

    Note: Remember to update the sha256 checksum information.

 2. Skip this step for non stable releases after GA of minor or major versions
    (e.g. announce 7.4.0RC1, but not 7.4.1RC1):

    Add a short notice to phpweb stating that there is a new release, and
    highlight the major important things (security fixes) and when it is
    important to upgrade.

    * Call `php bin/createNewsEntry` in your local phpweb checkout. Use category
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

## Rolling a stable release

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
    `web/php-distributions.git`, then update the git submodule reference in
    `web/php.git`:

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

    This is to fetch the last commit id from php-distributions.git and commit
    this last commit id to `web/php.git`, then, website will now sync.

12. Once the release has been tagged, contact release managers, Windows
    builders, and package maintainers so that they can build releases. Do not
    send this announcement to any public lists.

## Getting the stable release announced

 1. Update `phpweb/include/releases.inc` with the old release info (updates the
    download archives).

    * You can run `php bin/bumpRelease 7 4` where the first number is the major
      version, and the second number is the minor version (7.4 in this example).

    * If that fails for any non-trivially fixable reason, you can manually copy
      the old information to `include/releases.inc`.

 2. Update `phpweb/include/version.inc` (X_Y=major_minor release number):

    * `$PHP_X_Y_VERSION` to the correct version
    * `$PHP_X_Y_DATE` to the release date
    * `$PHP_X_Y_SHA256` array and update all the SHA256 sums
    * `$PHP_X_Y_TAGS` array should include `security` if this is a security
      release
    * Make sure there are no outdated "notes" or edited "date" keys in the
      `$RELEASES[X][$PHP_X_VERSION]["source"]` array.

 3. Create the release file (`releases/x_y_z.php`)

    Usually we use the same content as for point 6, but included in php template
    instead of the release xml.

 4. Update `php-qa/include/release-qa.php` and add the next version as an
    QARELEASE (prepare for next RC).

 5. Update the ChangeLog file for the given major version

    e.g. `ChangeLog-7.php` from the `NEWS` file

    * You may want to try `php-web/bin/news2html` to automate this task.

    * Go over the list and put every element on one line.
    * Check for `&`, `<` and `>` and escape them if necessary.
    * Remove all the names at the ends of lines.
    * For marking up, you can do the following (with `vi`):

        I. `s/^- /<li>/`

        II. `s/$/<\/li>/`

        III. `s/Fixed bug #\([0-9]\+\)/<?php bugfix(\1); ?>/`

        IV. `s/Fixed PECL bug #\([0-9]\+\)/<?php peclbugfix(\1); ?>/`

        V. `s/FR #\([0-9]\+\)/FR <?php bugl(\1); ?>/`

 6. Add a short notice to phpweb stating that there is a new release, and
    highlight the major important things (security fixes) and when it is
    important to upgrade.

    * Call `php bin/createNewsEntry` in your local phpweb checkout.
    * Use the "frontpage" and "releases" category.
    * Add the content for the news entry.

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

## Re-releasing the same version (or -pl)

 1. Commit the new binaries to `phpweb/distributions/`

 2. Update `phpweb/include/version.inc` (X_Y=major_minor release number):

    * If only releasing for one OS, make sure you edit only those variables.
    * `$PHP_X_Y_VERSION` to the correct version
    * `$PHP_X_Y_DATE` to the release date
    * `$PHP_X_Y_SHA256` array and update all the SHA256 sums
    * Make sure there are no outdated "notes" or edited "date" keys in the
      `$RELEASES[X][$PHP_X_VERSION]["source"]` array.

 3. Add a short notice to phpweb stating that there is a new release, and
    highlight the major important things (security fixes) and when it is
    important to upgrade.

    * Call `php bin/createNewsEntry` in your local phpweb checkout.
    * Add the content for the news entry.

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

    Example: https://git.php.net/?p=php-src.git;a=commit;h=a63c99b
    Push the new branch and the commit just added to master.

 3. Immediately notify internals@ of the branch cut and advise the new merging
    order. Example:

    http://news.php.net/php.internals/99903

 4. Update php-web:git.php and https://wiki.php.net/vcs/gitworkflow to reflect
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

## Prime the selection of the Release Managers of the next version

 1. About three months before the scheduled release of the first alpha of the
    next minor or major release, issue a call for volunteers on
    internals@lists.php.net (cf. http://news.php.net/php.internals/98652).

 2. Make sure that there are two or more volunteers, and hold a vote if
    necessary (see
    https://wiki.php.net/rfc/releaseprocess#release_managers_selection).

 3. Help the new release managers with their first steps.

## New Release Manager checklist

 1. Email systems@ to get setup for access to downloads.php.net and to be added
    to the release-managers@ distribution list.

 2. Create a GPG key for your @php.net address and publish it by editing
    `include/gpg-keys.inc` in the `web-php` repository, adding the output of
    `gpg --fingerprint "$USER@php.net"`. Let one or more of the previous RMs
    sign your key. Publish your public key to pgp.mit.edu with:
    `gpg --keyserver pgp.mit.edu --send-keys $KEYID`

 3. Request karma to edit `main/php_version.h` and `Zend/zend.h`. Possibly karma
    for other restricted parts of php-src might come in question. To edit
    `main/php_version.h` in a release branch, you need release manager karma in
    `global_avail`.

 4. Request karma for `web/qa.git` and `web/php.git` for publishing release
    announcements.

 5. Request moderation access to php-announce@lists.php.net and
    primary-qa-tester@lists.php.net lists, to be able to moderate your release
    announcements. All the announcements should ideally be sent from the
    @php.net alias. Note, that for sending emails as @php.net alias a custom
    SMTP server needs to be used.
