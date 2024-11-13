<?php
if ((LIBXSLT_VERSION === 10140 || LIBXSLT_VERSION === 10141)
    && LIBXML_VERSION < 21300) {
    die('skip upstream regression https://gitlab.gnome.org/GNOME/libxslt/-/issues/113');
}
