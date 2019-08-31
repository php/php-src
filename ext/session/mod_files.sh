#!/usr/bin/env bash

if [[ "$2" = "" ]] || [[ "$3" = "" ]]; then
       echo "Usage: $0 BASE_DIRECTORY DEPTH BITS_PER_CHAR"
       echo "BASE_DIRECTORY will be created if it doesn't exist"
       echo "DEPTH must be an integer number >0"
       echo "BITS_PER_CHAR(session.sid_bits_per_character) should be one of 4, 5, or 6."
       # http://php.net/manual/en/session.configuration.php#ini.session.sid-bits-per-character
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
bitsperchar="$3"

hash_chars="0 1 2 3 4 5 6 7 8 9 a b c d e f"

if [[ "$bitsperchar" -ge "5" ]]; then
       hash_chars="$hash_chars g h i j k l m n o p q r s t u v"
fi

if [[ "$bitsperchar" -ge "6" ]]; then
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


echo "Creating session path in $directory with a depth of $depth for session.sid_bits_per_character = $bitsperchar"

for i in $hash_chars; do
       newpath="$directory/$i"
       mkdir $newpath || exit 1
       bash $0 $newpath `expr $depth - 1` $bitsperchar recurse
done
