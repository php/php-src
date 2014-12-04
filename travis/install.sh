#!/bin/bash
if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
  sudo apt-get update -y;
  sudo cp ./travis/de /var/lib/locales/supported.d/de
  sudo dpkg-reconfigure locales
  sudo apt-get install re2c libgmp-dev libicu-dev libmcrypt-dev libtidy-dev
elif [[ $TRAVIS_OS_NAME == 'osx' ]]; then
  export HOMEBREW_SOURCEFORGE_MIRROR="heanet";
  brew update;
  # install dependencies for building php
  brew install bison27 re2c gmp4 icu4c mcrypt gettext readline;
  brew link --force gettext readline icu4c;
  pg_ctl -D /usr/local/var/postgres start
  brew install https://raw.githubusercontent.com/Homebrew/homebrew/8103ff98f9d4c6c99ef69a90f6999047695a436f/Library/Formula/mysql.rb
  unset TMPDIR
  mysql_install_db --verbose --user=`whoami` --basedir="$(brew --prefix mysql)" --datadir=/usr/local/var/mysql --tmpdir=/tmp
  mysql.server start
  mysql -u root -e "CREATE USER 'travis'@'localhost' IDENTIFIED BY '';  GRANT ALL PRIVILEGES ON *.* TO 'travis'@'localhost' WITH GRANT OPTION";
fi
