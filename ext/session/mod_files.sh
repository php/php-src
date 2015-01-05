#! /bin/sh

if [[ "$2" = "" ]] || [[ "$3" = "" ]]; then
       echo "Usage: $0 BASE_DIRECTORY DEPTH HASH_BITS"
       echo "BASE_DIRECTORY will be created if it doesn't exist"
       echo "DEPTH must be an integer number >0"
       echo "HASH_BITS(session.hash_bits_per_charactor) should be one of 4, 5, or 6"
       exit 1
fi

if [[ "$2" = "0" ]] && [[ ! "$4" = "recurse" ]]; then
       echo "Can't create a directory tree with depth of 0, exiting."
fi

if [[ "$2" = "0" ]]; then
       exit 0
fi

directory="$1"
depth="$2"
hashbits="$3"

hash_chars="0 1 2 3 4 5 6 7 8 9 a b c d e f"

if [[ "$hashbits" -ge "5" ]]; then
       hash_chars="$hash_chars g h i j k l m n o p q r s t u v"
fi

if [[ "$hashbits" -ge "6" ]]; then
       hash_chars="$hash_chars w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z - ,"
fi

while [[ -d $directory ]] && [[ $( ls $directory ) ]]; do
       echo "Directory $directory is not empty! What would you like to do?"

       options="\"Delete directory contents\" \"Choose another directory\" \"Quit\""
       eval set $options
       select opt in "$@"; do

              if [[ $opt = "Delete directory contents" ]]; then
                     echo "Deleting $directory contents... "
                     rm -rf $directory/*
              elif [[ $opt = "Choose another directory" ]]; then
                     echo "Which directory would you like to choose?"
                     read directory
              elif [[ $opt = "Quit" ]]; then
                     exit 0
              fi

              break;
       done
done

if [[ ! -d $directory ]]; then
       mkdir -p $directory
fi


echo "Creating session path in $directory with a depth of $depth for session.hash_bits_per_character = $hashbits"

for i in $hash_chars; do
       newpath="$directory/$i"
       mkdir $newpath || exit 1
       sh $0 $newpath `expr $depth - 1` $hashbits recurse
done
