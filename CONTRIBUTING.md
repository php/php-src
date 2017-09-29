# Contributing to PHP

Anybody who programs in PHP can be a contributing member of the community that
develops and deploys it; the task of deploying PHP, documentation and associated
websites is a never ending one. With every release, or release candidate comes a
wave of work, which takes a lot of organization and co-ordination.

You don't need any login accounts or special access to download, build, debug
and begin submitting PHP or PECL code, tests or documentation. Once you've
followed this document and had several patches accepted, commit privileges are
often quickly granted.

PHP and most PECL packages use Git for revision control. Some PECL packages use
Subversion (SVN). Read http://www.php.net/git.php for help on using Git to get
and build PHP source code. We recommend to look at our
[workflow](https://wiki.php.net/vcs/gitworkflow) and our
[Git FAQ](https://wiki.php.net/vcs/gitfaq).

## Reporting bugs

Bugs can be filed on the [PHP bug tracker](https://bugs.php.net/). If this is
the first time you've filed a bug, we suggest reading the
[guide to reporting a bug](https://bugs.php.net/how-to-report.php). Where possible,
please include a self-contained reproduction case.

## Feature requests

New feature requests are generally submitted in the form of
[Requests for Comments](https://wiki.php.net/rfc/howto), ideally accompanied by
[pull requests](#pull-requests). You can find the extremely large list of RFCs
that have been previously considered on the [PHP Wiki](https://wiki.php.net/rfc).

You may want to read
[The Mysterious PHP RFC Process](https://blogs.oracle.com/opal/entry/the_mysterious_php_rfc_process)
for additional notes on the best way to approach submitting an RFC.

## Pull requests

The preferred way to propose a PHP patch is sending a pull request from
[GitHub](https://github.com/php/php-src).

Simply fork the official PHP repository and send a pull request. A notification
will be sent to the pull request mailing list. Sending a note to
[PHP Internals list](mailto:internals@lists.php.net) may help getting more
feedback and quicker turnaround. You can also add pull requests to bug reports
at [bugs.php.net](http://bugs.php.net/).

PHP welcomes pull requests to [add tests](#writing-tests), fix bugs and to
implement RFCs. Please be sure to include tests as appropriate!

If you are fixing a bug, then please submit your PR against the lowest actively
supported branch of PHP that the bug affects (only green branches on
[the supported version page](http://php.net/supported-versions.php) are supported).
For example, at the time of writing in mid-2017, the lowest supported version is
PHP 7.0, which corresponds to the `PHP-7.0` branch in Git. Please also make sure
you add a link to the PR in the bug on [the bug tracker](https://bugs.php.net/).

Pull requests implementing RFCs should be submitted against `master` branch.

Pull requests should *never* be submitted against `PHP-x.y.z` branches, as these
are only used for release management when tagging the releases, hence they are
closed to the general public.

If your pull request exhibits conflicts with the base branch, please resolve them
by using `git rebase` instead of `git merge`.

## Writing tests

We love getting new tests! PHP is a huge project and improving code coverage is
a huge win for every PHP user.

Our QA site includes a page detailing
[how to write test cases](http://qa.php.net/write-test.php).

## Writing documentation

There are two ways to contribute to the PHP manual. You can edit the manual and
send patches anonymously via [the online editor](https://edit.php.net/), or you
can check the XML source out from Subversion and edit that and build it
[per the instructions on the documentation site](http://doc.php.net/tutorial/).
Patches created that way should be sent to the
[documentation mailing list](mailto:phpdoc@lists.php.net).

If you are fixing incorrect PHP documentation first create a bug or identify an
existing [bug](http://bugs.php.net/). A bug can be used to track the patch
progress and prevent your changes getting lost in the PHP mail archives.

If your change is large, then first discuss it with the mail list
phpdoc@lists.php.net. Subscription is explained on
http://php.net/mailing-lists.php.

Attach the patch to the PHP bug and consider sending a notification email about
the change to phpdoc@lists.php.net. Explain what has been fixed/added/changed by
your patch.

## Git rules

We'll assume you're basically familiar with [Git](http://git-scm.com/), but feel
free to post your questions on the mailing list.

PHP is developed through the efforts of a large number of people. Collaboration
is a Good Thing(tm), and Git lets us do this. Thus, following some basic rules
with regards to Git usage will:

* Make everybody happier, especially those responsible for maintaining PHP itself.
* Keep the changes consistently well documented and easily trackable.
* Prevent some of those *Oops* moments.
* Increase the general level of good will on planet Earth.

Having said that, here are the organizational rules:

* Respect other people working on the project.
* Discuss any significant changes on the list before committing and get
  confirmation from the release manager for the given branch.
* Look at [EXTENSIONS](/EXTENSIONS) file to see who is the primary maintainer of
  the code you want to contribute to.
* If you "strongly disagree" about something another person did, don't start
  fighting publicly - take it up in private email.
* If you don't know how to do something, ask first!
* Test your changes before committing them. To do so use `make test` and write
  tests. Submitting test scripts helps us to understand what functionality has
  changed. It is important for the stability and maintainability of PHP that
  tests are comprehensive.
* For development use the `--enable-debug` switch to avoid memory leaks and the
  `--enable-maintainer-zts` switch to ensure your code handles TSRM correctly and
  doesn't break for those who need that. Check the PHP and web server error logs
  after running your PHP tests.
* Update Git source from upstream before testing and submitting your patch.
* Review the patch once more just before submitting it.

The next few rules are more of a technical nature:

* Follow [CODING_STANDARDS](/CODING_STANDARDS) before you start working.
* All changes should first go to the lowest supported branch (i.e. 7.0) and then
  get merged up to all other branches. If a change is not needed for later branches
  (i.e. fixes for features which were dropped from later branches) an empty merge
  should be done.
* All news updates intended for public viewing, such as new features, bug fixes,
  improvements, etc., should go into the `NEWS` file of *any stable release*
  version with the given change. In other words, news about a bug fix which went
  into PHP-7.1, PHP-7.2 and master should be noted in PHP-7.1/NEWS and PHP-7.2/NEWS
  but not master, which is not a public released version yet. When you change
  the `NEWS` file for a bug fix, then please keep the bugs sorted in decreasing
  order under the fixed version. Unless you have commit access and you are going
  to commit it yourself, you shouldn't include the `NEWS` entry as it will
  probably create conflicts for the person merging it.
* Do not commit multiple files and dump all messages in one commit. If you
  modified several unrelated files, commit each group separately and provide a
  nice commit message for each one. See example below.
* Do write your commit message in such a way that it makes sense even without the
  corresponding diff. One should be able to look at it, and immediately know what
  was modified. Definitely include the function name in the message as shown below.
* In your commit messages, keep each line shorter than 80 characters. And try to
  align your lines vertically, if they wrap. It looks bad otherwise.
* If you modified a function that is callable from PHP, prepend PHP to the
  function name.
* Add in-line comments and/or have external documentation ready.

The format of the commit messages is pretty simple.

```
<max 79 characters short description>\n
\n
<long description, 79 chars per line>
\n
```

An example from the git project (commit 2b34e486bc):

```
pack-objects: Fix compilation with NO_PTHREDS

It looks like commit 99fb6e04 (pack-objects: convert to use
parse_options(), 2012-02-01) moved the #ifdef NO_PTHREDS around but
hasn't noticed that the 'arg' variable no longer is available.
```

If you fix some bugs, you should note the bug ID numbers in your commit message.
Bug ID should be prefixed by `#` for easier access to bug report when developers
are browsing VCS via LXR or Bonsai.

Example:

```
Fixed bug #14016 (pgsql notice handler double free crash bug.)
```

You can use [GitHub](https://github.com/php) and [gitweb](http://git.php.net/)
to look at PHP Git repositories in various ways.

## PHP source code

If you are fixing broken functionality in PHP C source code first create a bug
or identify an existing bug at http://bugs.php.net/. A bug can be used to track
the patch progress and prevent your changes getting lost in the PHP mail archives.

Information on PHP internal C functions is at http://php.net/internals. Various
external resources can be found on the web. See https://wiki.php.net/internals
for some references. A standard printed reference is the book "Extending and
Embedding PHP" by Sara Golemon. Lot of useful information is available also in
the [PHP Internals Book](http://www.phpinternalsbook.com/).

## Patches

If you are not using GitHub, you can also create patch files for PHP source code,
PHP documentation or PECL and attach your patch to a PHP bug and consider sending
a notification email about the change to a their dedicated mailing list. If the
bug is for an extension, also CC the extension maintainer. Explain what has been
changed by your patch. Test scripts should be included.

In this case please make the mail subject prefix `[PATCH]`. If attaching a patch,
ensure it has a file extension of `.txt`. This is because only MIME attachments
of type `text/*` are accepted.

After testing is finished, create a patch file using the command:

```
git diff > your_patch.txt
```

For ease of review and later troubleshooting, submit individual patches for each
bug or feature.

## PECL

If you are fixing broken functionality in a [PECL extension](http://pecl.php.net/)
then create a bug or identify an existing bug at http://bugs.php.net/. A bug can
be used to track the patch progress and prevent your changes getting lost in the
PHP mail archives.

If your change is large then create a [Request For Comment (RFC) page](http://wiki.php.net/rfc),
discuss it with the extension maintainer, and discuss it on the development mail
list pecl-dev@lists.php.net. PECL mail list subscription is explained on
http://pecl.php.net/support.php. RFC Wiki accounts can be requested on
http://wiki.php.net/start?do=register.

Update any open bugs and add a link to the source of your patch. Send the patch
or pointer to the bug to pecl-dev@lists.php.net. Also CC the extension maintainer.
Explain what has been changed by your patch. Test scripts should be included.

## PEAR

[PEAR](http://pear.php.net/) (PHP Extension and Application Repository) is a
framework and distribution system for reusable PHP components. Information on
contributing to PEAR is available at
* http://pear.php.net/manual/en/developers-newmaint.php and
* http://pear.php.net/manual/en/guide-developers.php

## Getting help

If you are having trouble contributing to PHP, or just want to talk to a human
about what you're working on, you can contact us via the
[internals mailing list](mailto:internals@lists.php.net), or the
[documentation mailing list](mailto:phpdoc@lists.php.net) for documentation
issues.

Although not a formal channel, you can also find a number of core developers on
the `#php.pecl` channel and similarly documentation writers can be found on
`#php.doc` on [EFnet](http://www.efnet.org/). For windows related discussions
there is also `#winphp-dev` on FreeNode.

### What happens after submitting your PHP, PHP documentation or PECL patch?

If your pull request or patch is easy to review and obviously has no side-effects,
it might be committed relatively quickly.

Because PHP is a volunteer-driven effort more complex patches will require
patience on your side. If you do not receive feedback in a few days, consider
resubmitting the patch. Before doing this think about these questions:

* Did I send the patch to the right mailing list?
* Did I review the mail list archives to see if these kind of changes had been
  discussed before?
* Did I explain my patch clearly?
* Is my patch too hard to review? Because of what factors?

### What happens when your patch is applied?

Your name will likely be included in the Git commit log. If your patch affects
end users, a brief description and your name might be added to the `NEWS` file.

Thank you for patching PHP! Happy hacking,

PHP Team
