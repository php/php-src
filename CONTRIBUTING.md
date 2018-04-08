# Contributing to PHP

Anybody who programs in PHP can be a contributing member of the community that
develops and deploys it; the task of deploying PHP, documentation and
associated websites is a never ending one. With every release, or release
candidate comes a wave of work, which takes a lot of organization and
co-ordination.

## Pull requests

PHP welcomes pull requests to [add tests](#writing-tests), fix bugs and to
implement RFCs. Please be sure to include tests as appropriate!

If you are fixing a bug, then please submit your PR against the lowest actively
supported branch of PHP that the bug affects (only green branches on
[the supported version page](http://php.net/supported-versions.php) are supported).
For example, at the time of writing in early-2018, the lowest supported version is
PHP 7.1, which corresponds to the `PHP-7.1` branch in Git. Please also make sure you
add a link to the PR in the bug on [the bug tracker](https://bugs.php.net/).

Pull requests implementing RFCs should be submitted against `master`.

Pull requests should *never* be submitted against `PHP-x.y.z` branches, as
these are only used for release management.

If your pull request exhibits conflicts with the base branch, please resolve them
by using `git rebase` instead of `git merge`.

## Filing bugs

Bugs can be filed on the [PHP bug tracker](https://bugs.php.net/). If this is
the first time you've filed a bug, we suggest reading the
[guide to reporting a bug](https://bugs.php.net/how-to-report.php).

Where possible, please include a self-contained reproduction case!

## Feature requests

Feature requests are generally submitted in the form of
[Requests for Comment](https://wiki.php.net/rfc/howto), ideally accompanied by
[pull requests](#pull-requests). You can find the extremely large list of RFCs
that have been previously considered on the
[PHP Wiki](https://wiki.php.net/rfc).

You may want to read
[The Mysterious PHP RFC Process](https://blogs.oracle.com/opal/entry/the_mysterious_php_rfc_process)
for additional notes on the best way to approach submitting an RFC.

## Writing tests

We love getting new tests! PHP is a huge project and improving code coverage is
a huge win for every PHP user.

[Our QA site includes a page detailing how to write test cases.](http://qa.php.net/write-test.php)
Please note that the section on submitting pull requests is outdated: in
addition to the process listed there, you can also
[submit pull requests](#pull-requests).

## Writing documentation

There are two ways to contribute to the PHP manual. You can edit the manual and
send patches anonymously via [the online editor](https://edit.php.net/), or you
can check the XML source out from Subversion and edit that and build it
[per the instructions on the documentation site](http://doc.php.net/tutorial/).
Patches created that way should be sent to the
[documentation mailing list](mailto:phpdoc@lists.php.net).

## Getting help

If you are having trouble contributing to PHP, or just want to talk to a human
about what you're working on, you can contact us via the
[internals mailing list](mailto:internals@lists.php.net), or the
[documentation mailing list](mailto:phpdoc@lists.php.net) for documentation
issues.

Although not a formal channel, you can also find a number of core developers on
the #php.pecl channel on [EFnet](http://www.efnet.org/). Similarly, many
documentation writers can be found on #php.doc.
