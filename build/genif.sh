#! /bin/sh

# replacement for genif.pl

infile=$1
shift
srcdir=$1
shift
extra_module_ptrs=$1
shift
awk=$1
shift

if test -z "$infile" || test -z "$srcdir"; then
	echo "please supply infile and srcdir" >&2
	exit 1
fi

header_list=
olddir=`pwd`
cd $srcdir

module_ptrs="$extra_module_ptrs`echo $@ | $awk -f ./build/order_by_dep.awk`"

for ext in ${1+"$@"} ; do
	ext_dir=`echo "$ext" | cut -d ';' -f 2`
	header_list="$header_list $ext_dir/*.h*"
done

includes=`$awk -f ./build/print_include.awk $header_list`

cd $olddir

cat $infile | \
	sed \
	-e "s'@EXT_INCLUDE_CODE@'$includes'" \
	-e "s'@EXT_MODULE_PTRS@'$module_ptrs'" \
	-e 's/@NEWLINE@/\
/g'
