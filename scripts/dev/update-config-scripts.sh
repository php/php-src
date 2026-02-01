#!/bin/sh
#
# Update the PHP autoconf bits.

# Go to project root directory
cd "$(CDPATH='' cd -- "$(dirname -- "$0")/../../" && pwd -P)" || exit

# For config.*, GNU recommendation for updates is to download from Savannah
# See: https://www.gnu.org/software/gettext/manual/html_node/config_002eguess.html

update_file() {
	old_ts=$("build/$1" -t)
	echo "Old $1 timestamp: $old_ts"
	url="https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=$1;hb=HEAD"
	wget -O "build/$1" "$url"
	new_ts=$("build/$1" -t)
	echo "New $1 timestamp: $new_ts"
}

update_file config.guess
update_file config.sub
