<?php
/* Header file for common file test functions
     Following functions are provided :
       create_files() : create files with specified contents
       delete_files() : delete files
       create_links() : crate links of different types
       delete_links() : delete links
       fill_files()   : fill file with specified contents
       change_file_perms() : Change permission of files
       fill_buffer()  : fills buffer with specified contents
       compare_self_stat() : compares the first 13 elements of the
                            stat with the corresponding named key values of
                            the same stat.
       compare_stats() : Compares two stat values

*/

const FILE_NOT_FOUND = 2;

/*
 Function: bool create_file(string $filename, string $mode = "w");
 Description: creates a new file using fopen() call
   $filename = Name of the file
   $mode = Mode as specified in fopen call, read documentation of fopen() call for more info

 Returns:
   true on success, false otherwise
*/
function create_file($filename, $mode = "w") {
  $file_handle = fopen ($filename, $mode);
  if ( $file_handle == false )
    return false;
  fclose($file_handle);
  return true;
}

/*
 Function : bool fill_buffer(string &$buffer, string $fill_type, int $fill_size);
 Description: Fills the $buffer with data as specified with requested size.
   $buffer = buffer to be filled
   $fill_type:
       "text" = fills with string of size $file_size
       "numeric" = fills with numeric value of size $file_size
       "text_with_new_line" = similar to "text" fill type but writes with new line
       "alphanumeric" = fills with alphnumeric values
 Returns: true on success, false on invalid fill type
*/
function fill_buffer(&$buffer, $fill_type, $fill_size) {

  if ( $fill_type == "text" ) {
    $data = "text ";
    $size_divider = strlen($data);
    $add_value = strlen($data);
  } else if ( $fill_type == "text_with_new_line" ) {
    $data = "line\nline of text\n";
    $size_divider = strlen($data);
    $add_value = strlen($data);
  } else if ( $fill_type == "alphanumeric" ) {
    $data = "ab12 ";
    $size_divider = strlen($data);
    $add_value = strlen($data);
  } else if ( $fill_type == "numeric" ) {
    $data = 2;
    $size_divider = 1;
    $add_value = 0;
  } else {
    // invalid fill type;
    return false;
  }

  $tmp_buff = str_repeat($data, ($fill_size/$size_divider) + $add_value );

  if ( strlen($tmp_buff) > $fill_size ) {
    $buffer = substr($tmp_buff, 0, $fill_size);
  } else {
    $buffer = $tmp_buff;
  }

  return true;
}

/*
 Function : bool fill_file(resource $file_handle, string $fill_type, string $file_size);
 Description: Fills the file with data as specified with requested size.
   $file_handle = file handle, opened with write options,
   $fill_type:
       "text" = fills with string of size $file_size
       "numeric" = fills with numeric value of size $file_size
       "empty" = no fill operation performed, returns true
       "text_with_new_line" = similar to "text" fill type but writes with new line
       "alphanumeric" = fills with alphnumeric values
 Returns: true on success, false on failure & invalid fill type
*/

function fill_file($file_handle, $fill_type, $file_size) {

  if ( $fill_type == "empty" ) {
    // no fill required, return true
    return true;
  } if ( $fill_type == "text" ) {
    $data = "text ";
    $size_divider = strlen($data);
    $add_value = strlen($data);
  } else if ( $fill_type == "text_with_new_line" ) {
    $data = "line\nline of text\n";
    $size_divider = strlen($data);
    $add_value = strlen($data);
  } else if ( $fill_type == "alphanumeric" ) {
    $data = "ab12 ";
    $size_divider = strlen($data);
    $add_value = strlen($data);
  } else if ( $fill_type == "numeric" ) {
    $data = 2;
    $size_divider = 1;
    $add_value = 0;
  } else {
    // invalid fill type;
    return false;
  }

  // write in terms of a chunk of 1 K to avoid memory size overflow
  $size = $file_size;
  $chunk_size = 1024;
  if ( $size > $chunk_size ) {
    $loop_count = 1;
    do {
      $loop_count ++;
      if ( $size <= $chunk_size ) {
        $chunk_size = $size;
      }
      $num_values = str_repeat($data, ($chunk_size/$size_divider) + $add_value );
      $bytes_written = fwrite($file_handle, $num_values, $chunk_size);
      if ( $bytes_written != $chunk_size ) {
            return false;
      }
      $size -= $chunk_size;
    } while ( $size > 0 );
  } else {
    $num_values = str_repeat($data, ($chunk_size/$size_divider) + $add_value );
    $bytes_written = fwrite($file_handle, $num_values, $file_size);
    if ( $bytes_written != $file_size ) {
      return false;
    }
  }

  // successful, return true
  return true;
}

/*
 Function: int change_file_perms(string $file_path, int $count = 1, int $perms = 0755,
                                 string $name_prefix = "file",
                                 string $name_suffix = 1, $file_extension = ".tmp");
 Description: changes file permission for given file(s).
   $file_path = dir path where file exists
   $count = no. of files, default is 1
   $perms = new permission of the file, similar to $mode args of chmod() call
   $name_prefix = common name prefix, default is "file"
   $name_suffix = suffix to end the common name given in name_prefix to create
      a unique name. default is 1.
   $file_extension = default is .tmp
 Returns:
   Integer, Count of total files permission changed.
*/
function change_file_perms($file_path,
                           $count = 1,
                           $perms = 0755,
                           $name_prefix = "file",
                           $name_suffix = 1,
                           $file_extension = ".tmp" )
{
  $changed = 0;

  if( $count <= 0 )
    return $changed;

  if ( $name_suffix <= 0)
    $name_suffix = 1;

  for($loop_counter = 1; $loop_counter <= $count; $loop_counter++) {
    $filename = $file_path."/".$name_prefix.$name_suffix.$file_extension;
    if( chmod($filename, $perms) )
      $changed++;
    $name_suffix++;
  }
  return $changed;
}

/*
 Function: array create_files( string $file_path,
                               int $count = 1,
                               string $content_type = "numeric",
                               int $permission = 0755,
                               int $size = 1,
                               string $mode = "w",
                               string $name_prefix = "file",
                               int $name_suffix = 1,
                               string $flag = "kilobytes"
                               string $file_extension = ".tmp"
                             );
 Description: Creates given number of files with specified mode and
   permissions. File is filled with content of size specified.
   $file_path = dir where files will be created
   $name_prefix = prefix to be used for names, name is suffix with a
     unique numeric value to make the file name unique, default = file
   $name_suffix = suffix to be used for the name, default = 1
   $count = total no. of files to be created, default = 1
   $mode = file open mode as specified in fopen() call. Do not use
     modes used for only reading the file. Default = "w"
   $permission = An octal number, This should be similar to $mode
     specified in chmod() call.
   $content_type = Specify type of the content to fill in the file.
     "numeric" = fill file with numeric values
     "text" = fill file with regular text
     "empty" = empty file
     "text_with_new_line" = similar to text fill type, but writes with new line char
     "alphanumeric" = fill file with alpha numeric text
     If improper $content type is specified, file is created as empty
   $size = size of the fill in terms of kilobyte, i.e size of the file.
           if $flag is specified as "byte", then then given size is taken in bytes
   $flag = specify if size has to be treated as no of total bytes or
           multiple of KB.
     "kilobytes" = take size in terms of multiple of KB
     "byte" = take size in terms of bytes
   $file_extension = default is .tmp

 Returns:
   An array with following key value pair:
     created => total file created
     filled => total files filled
     perms_changed => total files permission changed
*/
function create_files( $file_path,
                       $count = 1,
                       $content_type = "numeric",
                       $permission = 0755,
                       $size = 1,
                       $mode = "w",
                       $name_prefix = "file",
                       $name_suffix = 1,
                       $flag = "kilobytes",
                       $file_extension = ".tmp"
                     )
{
  $return_value = array('created' => 0, 'filled' => 0, 'perms_changed' => 0);

  //ensure that suffix is a +ve integer
  if ($name_suffix <= 0) {
    $name_suffix = 1;
  }

  // check for proper size
  if ( $size == 0 )
    return $return_value;

  // prepare the size based on flag
  $file_size = $size;
  if ( $flag == "kilobytes" ) {
    $file_size = $file_size * 1024;
  }

  $tmp_name_suffix = $name_suffix;
  // create the files with specified mode and permission
  for($file_created_count = 1; $file_created_count <= $count; $file_created_count ++) {
    $filename = $file_path."/".$name_prefix.$tmp_name_suffix.$file_extension;

    $status = create_file($filename, $mode);

    $tmp_name_suffix++;

    if ($status == true) {
      $return_value['created']++;
    }
    else {
      return $return_value;
    }
  }

  if ( $content_type == "empty" ) {
    $return_value['filled'] = $count;
  } else {
    // fill the file with specifiec type of data and size
    $tmp_name_suffix = $name_suffix;
    for($loop_counter = 1; $loop_counter <= $count; $loop_counter ++) {
      $filename = $file_path."/".$name_prefix.$tmp_name_suffix.$file_extension;
      $file_handle = fopen($filename, $mode);
      if($file_handle == false) {
        fclose($file_handle);
        return $return_value;
      } // end of if

      // call fill_file() to fill the file
      if( fill_file($file_handle, $content_type, $file_size) )
        $return_value['filled']++;

      fclose($file_handle);

      $tmp_name_suffix++;
    } // end of for
  }

  // change all file's permissions
  $return_value['perms_changed'] = change_file_perms($file_path, $count, $permission, $name_prefix,
                                                     $name_suffix, $file_extension);

  return $return_value;
}


/*
 Function: function create_links( $file_path,
               $filename,
                       $link_count = 1,
                       $link_type = "soft",
                       $link_size = 1024,
                       $link_name_prefix = "link",
                       $link_name_suffix = 1,
                       $link_file_content = "text",
                       $link_perms = 0755,
                       $link_file_extension = ".tmp"
                     );

 Description: Creates given number of links with specified mode and
   permissions.Link is filled with content of size specified.
   $file_path = location of the file and where links need to be created
   $link_name_prefix = prefix to be used for names, name is suffix with a
     unique numeric value to make the file name unique, default = link
   $link_name_suffix = suffix to be used for the name, default = 1
   $link_count = total no. of links to be created to given file, default = 1
   $link_perms = An octal number, This should be similar to $mode
     specified in chmod() call.
   $link_file_content = Type of the content to fill in the file.
     numeric = fill file with numeric values
     text = fill file with regular text
     text_with_new_line = same as text but new lines are written
     alphanumeric = fill with alphanumeric text
     If imporper $content type is specified, file is created as empty
   $size = size of the fill in terms of kilobyte, i.e size of the file.
   $link_type = type of the link to be created
      "soft" = soft link
      "hard" = hard link
   $filename = file used to create a link on

 Returns:
   An array with following key value pair:
     created => total file created
     filled => total files filled, always returned as 1
     perms_changed => total files permission changed
*/
function create_links($file_path,
                      $filename,
                      $link_count = 1,
                      $link_type = "soft",
                      $link_size = 1024,
                      $link_name_prefix = "link",
                      $link_name_suffix = 1,
                      $link_file_content = "text",
                      $link_perms = 0755,
                      $link_file_extension = ".tmp"
                     )
{
  $return_value = array('created' => 0, 'filled' => 0, 'perms_changed' => 0);
  $tmp_name_suffix = $link_name_suffix;
  $src_filename = $file_path."/".$filename;
  switch( $link_type ) {
    default :
    case "soft" :  // create a soft link
      for($link_created_count = 1; $link_created_count <= $link_count; $link_created_count++) {
        $linkname = $file_path."/".$link_name_prefix.$tmp_name_suffix.$link_file_extension;
        $status = symlink( $src_filename, $linkname);
        $tmp_name_suffix++;
        if ($status) {
          $return_value['created']++;
        }
        else {
          $return_value;
        }
      }
      break;

    case "hard" :  // create a hard link
      for($link_created_count = 1; $link_created_count <= $link_count; $link_created_count++) {
        $linkname = $file_path."/".$link_name_prefix.$tmp_name_suffix.$link_file_extension;
        $status = link($src_filename, $linkname);
        $tmp_name_suffix++;
        if ($status) {
          $return_value['created']++;
        }
        else {
          $return_value;
        }
      }
      break;
  }

  if ( $link_file_content == "empty" ) {
    $return_value['filled'] = 1;
    return $return_value;
  }

  // fill the file with specific type of data and size
  $tmp_name_suffix = $link_name_suffix;
  $linkname = $file_path."/".$link_name_prefix.$tmp_name_suffix.$link_file_extension;
  $file_handle = fopen($linkname, "w");
  if($file_handle == false) {
    return $return_value;
  } // end of if

  // call fill_file() to fill the file
  if( fill_file($file_handle, $link_file_content, $link_size) )
    $return_value['filled']++;

  // close the link
  fclose($file_handle);

  // change the permission of the link file, only if hard link.
  // this is not applicable to soft links
  if( $link_type == "hard" ) {
    $return_value['perms_changed'] = change_file_perms($file_path,
                                                        $link_count,
                                                        $link_perms,
                                                        $link_name_prefix,
                                                        $link_name_suffix,
                                                        $link_file_extension );
  }

  return $return_value;
}

/*
 Function: bool delete_file(string $filename);
 Description: delete a given file if exists
 Returns: true on success
          false on failure
          FILE_NOT_FOUND if file doesn't exist
*/
function delete_file($filename) {
  // check if file exists
  if ( file_exists($filename) ) {
    if ( unlink($filename) )
      return true;
    else
      return false;
  }
  return FILE_NOT_FOUND;
}

/*
 Function: array delete_files(string $file_path, int $count = 1, string $name_prefix = "file",
                              int name_suffix = 1, $file_extension = ".tmp" );
 Description: Deletes given number of files if exists.
   $file_path = location of the files
   $name_prefix = prefix for the filename, rest of the name is incremental(increment by 1 only)
     numeric starting from suffix up to count
   $count = number of files to be deleted
   $name_suffix = first numeric suffix in the name
 Returns: An array with following key/value pair
   deleted = Number of files deleted.
   notfound = Count of non existing file
   failed = Count of failed to delete
*/
function delete_files($file_path,
                      $count = 1,
                      $name_prefix = "file",
                      $name_suffix = 1,
                      $file_extension = ".tmp")
{
  $return_value = array ('deleted' => 0, 'notfound' => 0, 'failed' => 0);

  if ( $name_suffix < 1 )
    $name_suffix = 1;
  for($loop_counter = 1; $loop_counter <= $count; $loop_counter++) {
    $filename = $file_path."/".$name_prefix.$name_suffix.$file_extension;
    $name_suffix++;
    $status = delete_file($filename);
    if($status == true) {
      $return_value['deleted']++;
    } else if($status == FILE_NOT_FOUND) {
      $return_value['notfound']++;
    } else {
      $return_value['failed']++;
    }

  } // end of for
  return $return_value;
}

/*
 Function: array delete_links( $file_path,
                               $link_file_count,
                               $link_name_prefix,
                               $link_name_suffix,
                               $link_file_extension );
 Description: Deletes given number of links if exists. Uses delete_files() function
   $file_path = location of link files
   $link_file_count = Number of link files
   $link_name_prefix = prefix for the linkname, rest of the name is incremental(increment by 1 only)
     numeric starting from $link_name_suffix up to count
   $link_name_suffix = first numeric suffix in the name

 Returns: An array with following key/value pair
   deleted = Number of links deleted.
   notfound = Count of non existing link
   failed = Count of failed to delete
*/
function delete_links($file_path,
                      $link_file_count = 1,
                      $link_name_prefix = "link",
                      $link_name_suffix = 1,
                      $link_file_extension = ".tmp")
{
   // call the delete files to delete links
   $return_value = delete_files( $file_path,
                                 $link_file_count,
                                 $link_name_prefix,
                                 $link_name_suffix,
                                 $link_file_extension );
   return $return_value;
}



/*
 Prototype:
  function compare_self_stat( array $stat );
 Description:
  Compares the each of the first 13 values of the stat array with the
  corresponding next 13 values of the same stat for equality
  $stat = stat array

 Returns: true when all of them match, false otherwise
*/
function compare_self_stat( array $stat )
{
  //return value
  $return_value = true;

  // named keys present in a stat
  $string_keys = array("dev", "ino", "mode", "nlink", "uid", "gid",
                       "rdev", "size", "atime", "mtime", "ctime",
                       "blksize", "blocks");

  // first numeric key
  $key = 0;

  // compare the values in the stat, which are accessed using numeric key with
  // values accessed using string keys
  foreach($string_keys as $str_key)
  {
    if($stat[$key] != $stat[$str_key]) {
      echo "Error: stat[$key] doesn't match with stat[$str_key]\n";
      $flag = false;
      $key++;
    }
    else {
      $key++;
    }
  } // end of foreach

  // if the $return_value is false, i.e all the element do not match then
  // dump the stat array so that its easy to figure out the error
  if ($return_value == false ) {
    echo "\n Dumping stat array ...\n";
    var_dump($stat);
  }

  return $return_value;
}// end of compare_self_stat

/*
Prototype:
  function compare_stats( array $stat1, array $stat2, array $fields,
                          [string $op = "==", [ bool $flag = false] ]);
Description:
  Compares two stat values, stat value should be obtained by stat/lstat
  $stat1 = first stat array
  $stat2 = second stat array
  $op = type of the comparison to be perform between elements of stat1 and stat2
    "!=" compare for not equal
    "==" compare for equality
    ">"  if each element of stat1 is > than stat2
    "<"  if each element of stat1 is < than stat2
  $fields = contains the key of the elements that needs to be compared.
            type of the comparison is based on $op argument value
  $flag = specify true to dump the stat1 and stat2
*/

$all_stat_keys = array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                       "dev", "ino", "mode", "nlink", "uid", "gid",
                       "rdev", "size", "atime", "mtime", "ctime",
                       "blksize", "blocks");

function compare_stats($stat1, $stat2, $fields, $op = "==", $flag = false ) {
  // dump the stat if requested
  if ( $flag == true ) {
    var_dump($stat1);
    var_dump($stat2);
  }

  $result = true;

  // compare values of given key from each stat array
  for($index = 0; $index < count($fields); $index++)
  {
    switch( $op )
    {
       case "==":
         if ( $stat1[ $fields[$index] ] != $stat2[ $fields[$index] ] ) {
           $result = false;
           echo "Error: stat1 do not match with stat2 at key value: $fields[$index]\n";
         }
         break;

       case "!=":
         if ( $stat1[ $fields[$index] ] != $stat2[ $fields[$index] ] ) {
           // do nothing as its not equal, else will take care of if equal
         } else {
           $result = false;
           echo "Error: stat1 equals stat2 at key value: $fields[$index]\n";
         }
         break;

       case ">":
         if ( $stat1[ $fields[$index] ] <= $stat2[ $fields[$index] ] ) {
           $result = false;
           echo "Error: stat1 is not greater than stat2 at key value: $fields[$index]\n";
         }
         break;

       case "<":
         if ( $stat1[ $fields[$index] ] >= $stat2[ $fields[$index] ] ) {
           $result = false;
           echo "Error: stat1 is not lesser than stat2 at key value: $fields[$index]\n";
         }
         break;
    }
  }
  // if the result is false(i.e values are not as expected),
  // dump the stat array so that easy to figure out the error
  if ( $result == false ) {
    echo "\n Dumping stat array 1...\n";
    var_dump($stat1);
    echo "\n Dumping stat array 2...\n";
    var_dump($stat2);
  }

  return $result;
}

?>
