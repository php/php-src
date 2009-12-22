=======================
  PHP Release Process
=======================

General notes and tips
----------------------

1. Do not release on Fridays, Saturdays or Sundays
because the sysadmins can not upgrade stuff then.

2. Package the day before a release. So if the release is to be on Thursday,
package on Wednesday.

3. Ensure that Windows builds will work before packaging

4. Follow all steps to the letter. When unclear ask previous RM's (Derick/Ilia)
before proceeding. Ideally make sure that for the first releases one of the
previous RM's is around to answer questions. For the steps related to the
php/QA/bug websites try to have someone from the webmaster team (Bjori) on hand.

5. Verify the tags to be extra sure everything was tagged properly.

6. Moving extensions from/to PECL requires root level access to the CVS server.
Contact systems@php.net to get this taken care of.

Moving extensions from php-src to PECL
- Filesystem: cp -r php-src/ext/foo pecl/foo
- cvs rm php-src/ext/foo

If the extension is still usable or not dead, in cooperation with the extension
maintainers if any:
- create the pecl.php.net/foo package and its content, license, maintainer
- create the package.xml, commit
- release the package

Moving extensions from PECL to php-src
- Filesystem: cp -r pecl/foo php-src/ext/foo
OR depending on the wishes from the PECL extension maintainer.
- Filesystem: ln -s pecl/foo php-src/ext/foo

Rolling a non stable release (alpha/beta/RC)
--------------------------------------------

1. Check windows snapshot builder logs (http://snaps.php.net/win32/snapshot-STABLE.log f.e.)

2. Bump the version numbers in ``main/php_version.h``, ``configure.in`` and possibly ``NEWS``.
Do not use abbreviations for alpha and beta.

3. Commit those changes

4. run the "scripts/dev/credits" script in php-src and commit the changes in the
credits files in ext/standard.

5. tag the repository with the version f.e. "``cvs tag php_4_4_1RC1``"
(of course, you need to change that to the version you're rolling an RC for).

6. Bump up the version numbers in ``main/php_version.h``, ``configure.in``
and possibly ``NEWS`` again, to the **next** version. F.e. if the release
candidate was "4.4.1RC1" then the new one should be "4.4.1RC2-dev" - regardless
if we get a new RC or not. This is to make sure ``version_compare()`` can
correctly work.

7. Commit those changes

8. Log in onto the snaps box and go into the correct tree (f.e. the PHP_4_4
branch if you're rolling 4.4.x releases).

9. You do not have to update the tree, but of course you can with "``cvs up -dP``".

10. run: ``./makedist php 4.4.1RC1``, this will export the tree, create configure
and build two tarballs (one gz and one bz2).

11. Copy those two tarballs to www.php.net, in your homedir there should be a
directory "downloads/". Copy them into there, so that the system can generate
MD5 sums. If you do not have this directory, talk to Derick.

12. Now the RC can be found on http://downloads.php.net/yourname,
f.e. http://downloads.php.net/derick/

13. Once the release has been tagged, contact the PHP Windows development team
(internals-win@lists.php.net) so that Windows binaries can be created. Once
those are made, they should be placed into the same directory as the source snapshots.

Getting the non stable release (alpha/beta/RC) announced
--------------------------------------------------------

1. Send an email (see example here: http://news.php.net/php.internals/19486)
**To** ``internals@lists.php.net`` and ``php-general@lists.php.net`` lists
pointing out "the location of the release" and "the possible release date of
either the next RC, or the final release".

2. Send an email (see example here http://news.php.net/php.pear.qa/5201) **To**
``php-qa@lists.php.net`` and ``primary-qa-tests@lists.php.net``.
This email is to notify the selected projects about a new release so that they
can make sure their projects keep working. Make sure that you have been setup
as a moderator for ``primary-qa-tests@lists.php.net`` by having someone (Wez,
Derick) run the following commands for you:

``ssh lists.php.net``

``sudo -u ezmlm ezmlm-sub ~ezmlm/primary-qa-tester/mod moderator-email-address``

3. Update the MD5 sums in ``web/qa/trunk/include/rc-md5sums.txt`` (no empty lines).

4. Update in ``web/qa/trunk/include/release-qa.php`` constants with the new RC and
commit this.

 a. ``$BUILD_TEST_RELEASES`` = array("4.4.7RC1", "5.2.2RC1")

 b. ``$CURRENT_QA_RELEASE_4`` = "4.4.7RC1" (``$CURRENT_QA_RELEASE_5`` for PHP5)

 c. ``$RELEASE_PROCESS`` = array(4 => true, 5 => true)

1. Update in ``php-bugs/trunk/include/functions.inc`` the ``show_version_option``
function to include the new RC and commit.

2. Update ``phpweb/include/version.inc`` (x=major version number)

 a. ``$PHP_x_RC`` = "5.3.0RC1"

 b. ``$PHP_x_RC_DATE`` = "06 September 2007"

3. Commit those changes:

 a. ``cvs commit include/version.inc include/releases.inc``

4. For the first RC, write the doc team (phpdoc@lists.php.net) about updating the
INSTALL and win32/install.txt files which are generated from the PHP manual sources.

Rolling a stable release
------------------------

1. Check windows snapshot builder logs (http://snaps.php.net/win32/snapshot-STABLE.log f.e.)

2. Bump the version numbers in ``main/php_version.h``, ``configure.in`` and possibly ``NEWS``.

3. **Merge** all related sections in NEWS (f.e. merge the 4.4.1RC1 and 4.4.0 sections)

4. Commit those changes

5. run the "scripts/dev/credits" script in php-src and commit the changes in the
credits files in ext/standard.

6. tag the repository with the version f.e. "``cvs tag php_4_4_1``"
(of course, you need to change that to the version you're rolling an RC for).
When making 5.X release, you need to tag the Zend directory separately!!

7. Bump up the version numbers in ``main/php_version.h``, ``configure.in`` and
possibly ``NEWS`` again, to the **next** version. F.e. if the release candidate
was "4.4.1RC1" then the new one should be "4.4.1RC2-dev" - regardless if we get
a new RC or not. This is to make sure ``version_compare()`` can correctly work.

8. Commit those changes

9. Log in onto the snaps box and go into the correct tree (f.e. the PHP_4_4
branch if you're rolling 4.4.x releases).

10. You do not have to update the tree, but of course you can with "``cvs up -dP``".

11. run: ``./makedist php 4.4.1``, this will export the tree, create configure
and build two tarballs (one gz and one bz2).

12. Commit those two tarballs to CVS (phpweb/distributions)

13. Once the release has been tagged, contact the PHP Windows development team
(internals-win@lists.php.net) so that Windows binaries can be created. Once
those are made, they should be committed to CVS too.

14. Check if the pear files are updated (phar for 5.1+ or run pear/make-pear-bundle.php with 4.4)

15. When making a final release, also remind the PHP Windows development team
(internals-win@lists.php.net) to prepare the installer packages for Win32.

Getting the stable release announced
------------------------------------

1. Run the bumpRelease script for phpweb on your local checkout

 a. ``php bin/bumpRelease 5`` (or ``php bin/bumpRelease 4`` for PHP4)

2. Edit ``phpweb/include/version.inc`` and change (X=major release number):

 a. ``$PHP_X_VERSION`` to the correct version

 b. ``$PHP_X_DATE`` to the release date

 c. ``$PHP_X_MD5`` array and update all the md5 sums

 d. set ``$PHP_X_RC`` to false!

 e. Make sure there are no outdated "notes" or edited "date" keys in the
 ``$RELEASES[X][$PHP_X_VERSION]["source"]`` array

 f. if the windows builds aren't ready yet prefix the "windows" key with a dot (".windows")

3. Update the ChangeLog file for the given major version
f.e. ``ChangeLog-4.php`` from the NEWS file

 a. go over the list and put every element on one line

 b. check for &, < and > and escape them if necessary

 c. remove all the names at the ends of lines

 d. for marking up, you can do the following (with VI):

  I. ``s/^- /<li>/``

  II. ``s/$/<\/li>/``

  III. ``s/Fixed bug #\([0-9]\+\)/<?php bugfix(\1); ?>/``

  IV. ``s/Fixed PECL bug #\([0-9]\+\)/<?php peclbugfix(\1); ?>/``

  V. ``s/FR #\([0-9]\+\)/FR <?php bugl(\1); ?>/``

4. ``cp releases/4_4_0.php releases/4_4_1.php``

5. ``cvs add releases/4_4_1.php``

6. Update the ``releases/*.php`` file with relevant data. The release
announcement file should list in detail:

 a. security fixes,

 b. changes in behavior (whether due to a bug fix or not)

7. Add a short notice to phpweb stating that there is a new release, and
highlight the major important things (security fixes) and when it is important
to upgrade.

 a. Call php bin/createNewsEntry in your local phpweb checkout

 b. Add the content for the news entry

8. Commit all the changes.

9. Wait an hour or two, then send a mail to php-announce@lists.php.net,
php-general@lists.php.net and internals@lists.php.net with a text similar to
http://news.php.net/php.internals/17222.

10. Update ``php-bugs-web/include/functions.php`` to include the new version
number, and remove the RC from there.

11. Update ``qaweb/include/release-qa.php``

 a. Update the $BUILD_TEST_RELEASES array with the release name

 b. Update $RELEASE_PROCESS array (set to false)

  I. For PHP4: Set $CURRENT_QA_RELEASE_4 to false

  II. For PHP5: Set $CURRENT_QA_RELEASE_5 to false

Re-releasing the same version (or -pl)
--------------------------------------

1. Commit the new binaries to ``phpweb/distributions/``

2. Edit ``phpweb/include/version.inc`` and change (X=major release number):

 a. If only releasing for one OS, make sure you edit only those variables

 b. ``$PHP_X_VERSION`` to the correct version

 c. ``$PHP_X_DATE`` to the release date

 d. ``$PHP_X_MD5`` array and update all the md5 sums

 e. Make sure there are no outdated "notes" or edited "date" keys in the
 ``$RELEASES[X][$PHP_X_VERSION]["source"]`` array

3. Add a short notice to phpweb stating that there is a new release, and
highlight the major important things (security fixes) and when it is important
to upgrade.

 a. Call php bin/createNewsEntry in your local phpweb checkout

 b. Add the content for the news entry

4. Commit all the changes (``include/version.inc``, ``archive/archive.xml``,
``archive/entries/YYYY-MM-DD-N.xml``)

5. Wait an hour or two, then send a mail to php-announce@lists.php.net,
php-general@lists.php.net and internals@lists.php.net with a text similar to
the news entry.
