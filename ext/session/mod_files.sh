#! /bin/sh

if test "$2" = ""; then
	echo "usage: $0 basedir depth"
	exit 1
fi

if test "$2" = "0"; then
	exit 0
fi

if test -z "$3"; then
        param3=0

	echo "Are you sure to create sessions tree dirs
in $1 root path with $2 depth? (Y|N)" 
	read input
	if test "$input" != "Y"; then
		echo "\nexit without process"
		exit 0
	fi
	echo "\nProcessing..."
else
        param3=$3
fi


hash_chars="0 1 2 3 4 5 6 7 8 9 a b c d e f"
if test "$param3" -a "$param3" -ge "5"; then
  hash_chars="$hash_chars g h i j k l m n o p q r s t u v"
  if test "$param3" -eq "6"; then
    hash_chars="$hash_chars w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z - ,"
  fi
fi

for i in $hash_chars; do
	newpath="$1/$i"
	mkdir $newpath || exit 1
	sh $0 $newpath `expr $2 - 1` $param3
done

echo "\nDone..."
