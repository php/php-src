<?php
/* vim: set ts=4 sw=4: */
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Vincent Blavet <vincent@blavet.net>                          |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';

/**
* Creates a (compressed) Tar archive
*
* @author   Vincent Blavet <vincent@blavet.net>
* @version  $Revision$
* @package  Archive
*/
class Archive_Tar extends PEAR
{
    /**
    * @var string Name of the Tar
    */
    var $_tarname='';

    /**
    * @var boolean if true, the Tar file will be gzipped
    */
    var $_compress=false;

    /**
    * @var file descriptor
    */
    var $_file=0;

    /**
    * @var string Local Tar name of a remote Tar (http:// or ftp://)
    */
    var $_temp_tarname='';

    // {{{ constructor
    /**
    * Archive_Tar Class constructor. This flavour of the constructor only
    * declare a new Archive_Tar object, identifying it by the name of the
    * tar file.
    * If the compress argument is set the tar will be read or created as a
    * gzip compressed TAR file.
    *
    * @param    string  $p_tarname  The name of the tar archive to create
    * @param    boolean $p_compress if true, the archive will be gezip(ped)
    * @access public
    */
    function Archive_Tar($p_tarname, $p_compress = null)
    {
        $this->PEAR();
        if ($p_compress === null) {
            if (@file_exists($p_tarname)) {
                if ($fp = @fopen($p_tarname, "r")) {
                    // look for gzip magic cookie
                    $data = fread($fp, 2);
                    if ($data == "\37\213") {
                        $p_compress = true;
                    }
                }
            } else {
                // probably a remote file or some file accessible
                // through a stream interface
                if (substr($p_tarname, -2) == 'gz') {
                    $p_compress = true;
                }
            }
        }
        $this->_tarname = $p_tarname;
        if ($p_compress) { // assert zlib extension support
            $extname = 'zlib';
            if (!extension_loaded($extname)) {
                if (OS_WINDOWS) {
                    @dl("php_$extname.dll");
                } else {
                    @dl("$extname.so");
                }
            }
            if (!extension_loaded($extname)) {
                die("The extension '$extname' couldn't be found.\n".
                    "Please make sure your version of PHP was built".
                    "with '$extname' support.\n");
                return false;
            }
        }
        $this->_compress = (bool)$p_compress;
    }
    // }}}

    // {{{ destructor
    function _Archive_Tar()
    {
        $this->_close();
        // ----- Look for a local copy to delete
        if ($this->_temp_tarname != '')
            @unlink($this->_temp_tarname);
        $this->_PEAR();
    }
    // }}}

    // {{{ create()
    /**
    * This method creates the archive file and add the files / directories
    * that are listed in $p_filelist.
    * If a file with the same name exist and is writable, it is replaced
    * by the new tar.
    * The method return false and a PEAR error text.
    * The $p_filelist parameter can be an array of string, each string
    * representing a filename or a directory name with their path if
    * needed. It can also be a single string with names separated by a
    * single blank.
    * For each directory added in the archive, the files and
    * sub-directories are also added.
    * See also createModify() method for more details.
    *
    * @param array  $p_filelist An array of filenames and directory names, or a single
    *                           string with names separated by a single blank space.
    * @return                   true on success, false on error.
    * @see createModify()
    * @access public
    */
    function create($p_filelist)
    {
        return $this->createModify($p_filelist, '', '');
    }
    // }}}

    // {{{ add()
    /**
    * This method add the files / directories that are listed in $p_filelist in
    * the archive. If the archive does not exist it is created.
    * The method return false and a PEAR error text.
    * The files and directories listed are only added at the end of the archive,
    * even if a file with the same name is already archived.
    * See also createModify() method for more details.
    *
    * @param array  $p_filelist An array of filenames and directory names, or a single
    *                           string with names separated by a single blank space.
    * @return                   true on success, false on error.
    * @see createModify()
    * @access public
    */
    function add($p_filelist)
    {
        return $this->addModify($p_filelist, '', '');
    }
    // }}}

    // {{{ extract()
    function extract($p_path='')
    {
        return $this->extractModify($p_path, '');
    }
    // }}}

    // {{{ listContent()
    function listContent()
    {
        $v_list_detail = array();

        if ($this->_openRead()) {
            if (!$this->_extractList('', $v_list_detail, "list", '', '')) {
                unset($v_list_detail);
                return(0);
            }
            $this->_close();
        }

        return $v_list_detail;
    }
    // }}}

    // {{{ createModify()
    /**
    * This method creates the archive file and add the files / directories
    * that are listed in $p_filelist.
    * If the file already exists and is writable, it is replaced by the
    * new tar. It is a create and not an add. If the file exists and is
    * read-only or is a directory it is not replaced. The method return
    * false and a PEAR error text.
    * The $p_filelist parameter can be an array of string, each string
    * representing a filename or a directory name with their path if
    * needed. It can also be a single string with names separated by a
    * single blank.
    * The path indicated in $p_remove_dir will be removed from the
    * memorized path of each file / directory listed when this path
    * exists. By default nothing is removed (empty path '')
    * The path indicated in $p_add_dir will be added at the beginning of
    * the memorized path of each file / directory listed. However it can
    * be set to empty ''. The adding of a path is done after the removing
    * of path.
    * The path add/remove ability enables the user to prepare an archive
    * for extraction in a different path than the origin files are.
    * See also addModify() method for file adding properties.
    *
    * @param array  $p_filelist     An array of filenames and directory names, or a single
    *                               string with names separated by a single blank space.
    * @param string $p_add_dir      A string which contains a path to be added to the
    *                               memorized path of each element in the list.
    * @param string $p_remove_dir   A string which contains a path to be removed from
    *                               the memorized path of each element in the list, when
    *                               relevant.
    * @return boolean               true on success, false on error.
    * @access public
    * @see addModify()
    */
    function createModify($p_filelist, $p_add_dir, $p_remove_dir='')
    {
        $v_result = true;

        if (!$this->_openWrite())
            return false;

        if ($p_filelist != '') {
            if (is_array($p_filelist))
                $v_list = $p_filelist;
            elseif (is_string($p_filelist))
                $v_list = explode(" ", $p_filelist);
            else {
                $this->_cleanFile();
                $this->_error('Invalid file list');
                return false;
            }

            $v_result = $this->_addList($v_list, $p_add_dir, $p_remove_dir);
        }

        if ($v_result) {
            $this->_writeFooter();
            $this->_close();
        } else
            $this->_cleanFile();

        return $v_result;
    }
    // }}}

    // {{{ addModify()
    /**
    * This method add the files / directories listed in $p_filelist at the
    * end of the existing archive. If the archive does not yet exists it
    * is created.
    * The $p_filelist parameter can be an array of string, each string
    * representing a filename or a directory name with their path if
    * needed. It can also be a single string with names separated by a
    * single blank.
    * The path indicated in $p_remove_dir will be removed from the
    * memorized path of each file / directory listed when this path
    * exists. By default nothing is removed (empty path '')
    * The path indicated in $p_add_dir will be added at the beginning of
    * the memorized path of each file / directory listed. However it can
    * be set to empty ''. The adding of a path is done after the removing
    * of path.
    * The path add/remove ability enables the user to prepare an archive
    * for extraction in a different path than the origin files are.
    * If a file/dir is already in the archive it will only be added at the
    * end of the archive. There is no update of the existing archived
    * file/dir. However while extracting the archive, the last file will
    * replace the first one. This results in a none optimization of the
    * archive size.
    * If a file/dir does not exist the file/dir is ignored. However an
    * error text is send to PEAR error.
    * If a file/dir is not readable the file/dir is ignored. However an
    * error text is send to PEAR error.
    * If the resulting filename/dirname (after the add/remove option or
    * not) string is greater than 99 char, the file/dir is
    * ignored. However an error text is send to PEAR error.
    *
    * @param array      $p_filelist     An array of filenames and directory names, or a single
    *                                   string with names separated by a single blank space.
    * @param string     $p_add_dir      A string which contains a path to be added to the
    *                                   memorized path of each element in the list.
    * @param string     $p_remove_dir   A string which contains a path to be removed from
    *                                   the memorized path of each element in the list, when
    *                                   relevant.
    * @return                           true on success, false on error.
    * @access public
    */
    function addModify($p_filelist, $p_add_dir, $p_remove_dir='')
    {
        $v_result = true;

        if (!@is_file($this->_tarname))
            $v_result = $this->createModify($p_filelist, $p_add_dir, $p_remove_dir);
        else {
            if (is_array($p_filelist))
                $v_list = $p_filelist;
            elseif (is_string($p_filelist))
                $v_list = explode(" ", $p_filelist);
            else {
                $this->_error('Invalid file list');
                return false;
            }

            $v_result = $this->_append($v_list, $p_add_dir, $p_remove_dir);
        }

        return $v_result;
    }
    // }}}

    // {{{ extractModify()
    /**
    * This method extract all the content of the archive in the directory
    * indicated by $p_path. When relevant the memorized path of the
    * files/dir can be modified by removing the $p_remove_path path at the
    * beginning of the file/dir path.
    * While extracting a file, if the directory path does not exists it is
    * created.
    * While extracting a file, if the file already exists it is replaced
    * without looking for last modification date.
    * While extracting a file, if the file already exists and is write
    * protected, the extraction is aborted.
    * While extracting a file, if a directory with the same name already
    * exists, the extraction is aborted.
    * While extracting a directory, if a file with the same name already
    * exists, the extraction is aborted.
    * While extracting a file/directory if the destination directory exist
    * and is write protected, or does not exist but can not be created,
    * the extraction is aborted.
    * If after extraction an extracted file does not show the correct
    * stored file size, the extraction is aborted.
    * When the extraction is aborted, a PEAR error text is set and false
    * is returned. However the result can be a partial extraction that may
    * need to be manually cleaned.
    *
    * @param string $p_path         The path of the directory where the files/dir need to by
    *                               extracted.
    * @param string $p_remove_path  Part of the memorized path that can be removed if
    *                               present at the beginning of the file/dir path.
    * @return boolean               true on success, false on error.
    * @access public
    * @see extractList()
    */
    function extractModify($p_path, $p_remove_path)
    {
        $v_result = true;
        $v_list_detail = array();

        if ($v_result = $this->_openRead()) {
            $v_result = $this->_extractList($p_path, $v_list_detail, "complete", 0, $p_remove_path);
            $this->_close();
        }

        return $v_result;
    }
    // }}}

    // {{{ extractList()
    /**
    * This method extract from the archive only the files indicated in the
    * $p_filelist. These files are extracted in the current directory or
    * in the directory indicated by the optional $p_path parameter.
    * If indicated the $p_remove_path can be used in the same way as it is
    * used in extractModify() method.
    * @param array  $p_filelist     An array of filenames and directory names, or a single
    *                               string with names separated by a single blank space.
    * @param string $p_path         The path of the directory where the files/dir need to by
    *                               extracted.
    * @param string $p_remove_path  Part of the memorized path that can be removed if
    *                               present at the beginning of the file/dir path.
    * @return                       true on success, false on error.
    * @access public
    * @see extractModify()
    */
    function extractList($p_filelist, $p_path='', $p_remove_path='')
    {
        $v_result = true;
        $v_list_detail = array();

        if (is_array($p_filelist))
            $v_list = $p_filelist;
        elseif (is_string($p_filelist))
            $v_list = explode(" ", $p_filelist);
        else {
            $this->_error('Invalid string list');
            return false;
        }

        if ($v_result = $this->_openRead()) {
            $v_result = $this->_extractList($p_path, $v_list_detail, "complete", $v_list, $p_remove_path);
            $this->_close();
        }

        return $v_result;
    }
    // }}}

    // {{{ _error()
    function _error($p_message)
    {
        // ----- To be completed
        $this->raiseError($p_message);
    }
    // }}}

    // {{{ _warning()
    function _warning($p_message)
    {
        // ----- To be completed
        $this->raiseError($p_message);
    }
    // }}}

    // {{{ _openWrite()
    function _openWrite()
    {
        if ($this->_compress)
            $this->_file = @gzopen($this->_tarname, "w");
        else
            $this->_file = @fopen($this->_tarname, "w");

        if ($this->_file == 0) {
            $this->_error('Unable to open in write mode \''.$this->_tarname.'\'');
            return false;
        }

        return true;
    }
    // }}}

    // {{{ _openRead()
    function _openRead()
    {
        if (strtolower(substr($this->_tarname, 0, 7)) == 'http://') {

          // ----- Look if a local copy need to be done
          if ($this->_temp_tarname == '') {
              $this->_temp_tarname = uniqid('tar').'.tmp';
              if (!$v_file_from = @fopen($this->_tarname, 'rb')) {
                $this->_error('Unable to open in read mode \''.$this->_tarname.'\'');
                $this->_temp_tarname = '';
                return false;
              }
              if (!$v_file_to = @fopen($this->_temp_tarname, 'wb')) {
                $this->_error('Unable to open in write mode \''.$this->_temp_tarname.'\'');
                $this->_temp_tarname = '';
                return false;
              }
              while ($v_data = @fread($v_file_from, 1024))
                  @fwrite($v_file_to, $v_data);
              @fclose($v_file_from);
              @fclose($v_file_to);
          }

          // ----- File to open if the local copy
          $v_filename = $this->_temp_tarname;

        } else
          // ----- File to open if the normal Tar file
          $v_filename = $this->_tarname;

        if ($this->_compress)
            $this->_file = @gzopen($v_filename, "rb");
        else
            $this->_file = @fopen($v_filename, "rb");

        if ($this->_file == 0) {
            $this->_error('Unable to open in read mode \''.$v_filename.'\'');
            return false;
        }

        return true;
    }
    // }}}

    // {{{ _openReadWrite()
    function _openReadWrite()
    {
        if ($this->_compress)
            $this->_file = @gzopen($this->_tarname, "r+b");
        else
            $this->_file = @fopen($this->_tarname, "r+b");

        if ($this->_file == 0) {
            $this->_error('Unable to open in read/write mode \''.$this->_tarname.'\'');
            return false;
        }

        return true;
    }
    // }}}

    // {{{ _close()
    function _close()
    {
        if (isset($this->_file)) {
            if ($this->_compress)
                @gzclose($this->_file);
            else
                @fclose($this->_file);

            $this->_file = 0;
        }

        // ----- Look if a local copy need to be erase
        // Note that it might be interesting to keep the url for a time : ToDo
        if ($this->_temp_tarname != '') {
            @unlink($this->_temp_tarname);
            $this->_temp_tarname = '';
        }

        return true;
    }
    // }}}

    // {{{ _cleanFile()
    function _cleanFile()
    {
        $this->_close();

        // ----- Look for a local copy
        if ($this->_temp_tarname != '') {
            // ----- Remove the local copy but not the remote tarname
            @unlink($this->_temp_tarname);
            $this->_temp_tarname = '';
        } else {
            // ----- Remove the local tarname file
            @unlink($this->_tarname);
        }
        $this->_tarname = '';

        return true;
    }
    // }}}

    // {{{ _writeFooter()
    function _writeFooter()
    {
      if ($this->_file) {
          // ----- Write the last 0 filled block for end of archive
          $v_binary_data = pack("a512", '');
          if ($this->_compress)
            @gzputs($this->_file, $v_binary_data);
          else
            @fputs($this->_file, $v_binary_data);
      }
      return true;
    }
    // }}}

    // {{{ _addList()
    function _addList($p_list, $p_add_dir, $p_remove_dir)
    {
      $v_result=true;
      $v_header = array();

      // ----- Remove potential windows directory separator
      $p_add_dir = $this->_translateWinPath($p_add_dir);
      $p_remove_dir = $this->_translateWinPath($p_remove_dir, false);

      if (!$this->_file) {
          $this->_error('Invalid file descriptor');
          return false;
      }

      if (sizeof($p_list) == 0)
          return true;

      for ($j=0; ($j<count($p_list)) && ($v_result); $j++) {
        $v_filename = $p_list[$j];

        // ----- Skip the current tar name
        if ($v_filename == $this->_tarname)
            continue;

        if ($v_filename == '')
            continue;

        if (!file_exists($v_filename)) {
            $this->_warning("File '$v_filename' does not exist");
            continue;
        }

        // ----- Add the file or directory header
        if (!$this->_addFile($v_filename, $v_header, $p_add_dir, $p_remove_dir))
            return false;

        if (@is_dir($v_filename)) {
            if (!($p_hdir = opendir($v_filename))) {
                $this->_warning("Directory '$v_filename' can not be read");
                continue;
            }
            $p_hitem = readdir($p_hdir); // '.' directory
            $p_hitem = readdir($p_hdir); // '..' directory
            while ($p_hitem = readdir($p_hdir)) {
                if ($v_filename != ".")
                    $p_temp_list[0] = $v_filename.'/'.$p_hitem;
                else
                    $p_temp_list[0] = $p_hitem;

                $v_result = $this->_addList($p_temp_list, $p_add_dir, $p_remove_dir);
            }

            unset($p_temp_list);
            unset($p_hdir);
            unset($p_hitem);
        }
      }

      return $v_result;
    }
    // }}}

    // {{{ _addFile()
    function _addFile($p_filename, &$p_header, $p_add_dir, $p_remove_dir)
    {
      if (!$this->_file) {
          $this->_error('Invalid file descriptor');
          return false;
      }

      if ($p_filename == '') {
          $this->_error('Invalid file name');
          return false;
      }

      // ----- Calculate the stored filename
      $p_filename = $this->_translateWinPath($p_filename, false);;
      $v_stored_filename = $p_filename;
      if (strcmp($p_filename, $p_remove_dir) == 0) {
          return true;
      }
      if ($p_remove_dir != '') {
          if (substr($p_remove_dir, -1) != '/')
              $p_remove_dir .= '/';

          if (substr($p_filename, 0, strlen($p_remove_dir)) == $p_remove_dir)
              $v_stored_filename = substr($p_filename, strlen($p_remove_dir));
      }
      $v_stored_filename = $this->_translateWinPath($v_stored_filename);
      if ($p_add_dir != '') {
          if (substr($p_add_dir, -1) == '/')
              $v_stored_filename = $p_add_dir.$v_stored_filename;
          else
              $v_stored_filename = $p_add_dir.'/'.$v_stored_filename;
      }

      $v_stored_filename = $this->_pathReduction($v_stored_filename);

      if (strlen($v_stored_filename) > 99) {
          $this->_warning("Stored file name is too long (max. 99) : '$v_stored_filename'");
          fclose($v_file);
          return true;
      }

      if (is_file($p_filename)) {
          if (($v_file = @fopen($p_filename, "rb")) == 0) {
              $this->_warning("Unable to open file '$p_filename' in binary read mode");
              return true;
          }

          if (!$this->_writeHeader($p_filename, $v_stored_filename))
              return false;

          while (($v_buffer = fread($v_file, 512)) != '') {
              $v_binary_data = pack("a512", "$v_buffer");
              if ($this->_compress)
                  @gzputs($this->_file, $v_binary_data);
              else
                  @fputs($this->_file, $v_binary_data);
          }

          fclose($v_file);

      } else {
          // ----- Only header for dir
          if (!$this->_writeHeader($p_filename, $v_stored_filename))
              return false;
      }

      return true;
    }
    // }}}

    // {{{ _writeHeader()
    function _writeHeader($p_filename, $p_stored_filename)
    {
        if ($p_stored_filename == '')
            $p_stored_filename = $p_filename;
        $v_reduce_filename = $this->_pathReduction($p_stored_filename);

        $v_info = stat($p_filename);
        $v_uid = sprintf("%6s ", DecOct($v_info[4]));
        $v_gid = sprintf("%6s ", DecOct($v_info[5]));
        $v_perms = sprintf("%6s ", DecOct(fileperms($p_filename)));

        $v_mtime = sprintf("%11s", DecOct(filemtime($p_filename)));

        if (@is_dir($p_filename)) {
          $v_typeflag = "5";
          $v_size = sprintf("%11s ", DecOct(0));
        } else {
          $v_typeflag = '';
          clearstatcache();
          $v_size = sprintf("%11s ", DecOct(filesize($p_filename)));
        }

        $v_linkname = '';

        $v_magic = '';

        $v_version = '';

        $v_uname = '';

        $v_gname = '';

        $v_devmajor = '';

        $v_devminor = '';

        $v_prefix = '';

        $v_binary_data_first = pack("a100a8a8a8a12A12", $v_reduce_filename, $v_perms, $v_uid, $v_gid, $v_size, $v_mtime);
        $v_binary_data_last = pack("a1a100a6a2a32a32a8a8a155a12", $v_typeflag, $v_linkname, $v_magic, $v_version, $v_uname, $v_gname, $v_devmajor, $v_devminor, $v_prefix, '');

        // ----- Calculate the checksum
        $v_checksum = 0;
        // ..... First part of the header
        for ($i=0; $i<148; $i++)
            $v_checksum += ord(substr($v_binary_data_first,$i,1));
        // ..... Ignore the checksum value and replace it by ' ' (space)
        for ($i=148; $i<156; $i++)
            $v_checksum += ord(' ');
        // ..... Last part of the header
        for ($i=156, $j=0; $i<512; $i++, $j++)
            $v_checksum += ord(substr($v_binary_data_last,$j,1));

        // ----- Write the first 148 bytes of the header in the archive
        if ($this->_compress)
            @gzputs($this->_file, $v_binary_data_first, 148);
        else
            @fputs($this->_file, $v_binary_data_first, 148);

        // ----- Write the calculated checksum
        $v_checksum = sprintf("%6s ", DecOct($v_checksum));
        $v_binary_data = pack("a8", $v_checksum);
        if ($this->_compress)
          @gzputs($this->_file, $v_binary_data, 8);
        else
          @fputs($this->_file, $v_binary_data, 8);

        // ----- Write the last 356 bytes of the header in the archive
        if ($this->_compress)
            @gzputs($this->_file, $v_binary_data_last, 356);
        else
            @fputs($this->_file, $v_binary_data_last, 356);

        return true;
    }
    // }}}

    // {{{ _readHeader()
    function _readHeader($v_binary_data, &$v_header)
    {
        if (strlen($v_binary_data)==0) {
            $v_header['filename'] = '';
            return true;
        }

        if (strlen($v_binary_data) != 512) {
            $v_header['filename'] = '';
            $this->_error('Invalid block size : '.strlen($v_binary_data));
            return false;
        }

        // ----- Calculate the checksum
        $v_checksum = 0;
        // ..... First part of the header
        for ($i=0; $i<148; $i++)
            $v_checksum+=ord(substr($v_binary_data,$i,1));
        // ..... Ignore the checksum value and replace it by ' ' (space)
        for ($i=148; $i<156; $i++)
            $v_checksum += ord(' ');
        // ..... Last part of the header
        for ($i=156; $i<512; $i++)
           $v_checksum+=ord(substr($v_binary_data,$i,1));

        $v_data = unpack("a100filename/a8mode/a8uid/a8gid/a12size/a12mtime/a8checksum/a1typeflag/a100link/a6magic/a2version/a32uname/a32gname/a8devmajor/a8devminor", $v_binary_data);

        // ----- Extract the checksum
        $v_header['checksum'] = OctDec(trim($v_data['checksum']));
        if ($v_header['checksum'] != $v_checksum) {
            $v_header['filename'] = '';

            // ----- Look for last block (empty block)
            if (($v_checksum == 256) && ($v_header['checksum'] == 0))
                return true;

            $this->_error('Invalid checksum : '.$v_checksum.' calculated, '.$v_header['checksum'].' expected');
            return false;
        }

        // ----- Extract the properties
        $v_header['filename'] = trim($v_data['filename']);
        $v_header['mode'] = OctDec(trim($v_data['mode']));
        $v_header['uid'] = OctDec(trim($v_data['uid']));
        $v_header['gid'] = OctDec(trim($v_data['gid']));
        $v_header['size'] = OctDec(trim($v_data['size']));
        $v_header['mtime'] = OctDec(trim($v_data['mtime']));
        if (($v_header['typeflag'] = $v_data['typeflag']) == "5") {
          $v_header['size'] = 0;
        }
        /* ----- All these fields are removed form the header because they do not carry interesting info
        $v_header[link] = trim($v_data[link]);
        $v_header[magic] = trim($v_data[magic]);
        $v_header[version] = trim($v_data[version]);
        $v_header[uname] = trim($v_data[uname]);
        $v_header[gname] = trim($v_data[gname]);
        $v_header[devmajor] = trim($v_data[devmajor]);
        $v_header[devminor] = trim($v_data[devminor]);
        */

        return true;
    }
    // }}}

    // {{{ _extractList()
    function _extractList($p_path, &$p_list_detail, $p_mode, $p_file_list, $p_remove_path)
    {
    $v_result=true;
    $v_nb = 0;
    $v_extract_all = true;
    $v_listing = false;

    $p_path = $this->_translateWinPath($p_path, false);
    if ($p_path == '' || (substr($p_path, 0, 1) != '/' && substr($p_path, 0, 3) != "../" && !strpos($p_path, ':'))) {
      $p_path = "./".$p_path;
    }
    $p_remove_path = $this->_translateWinPath($p_remove_path);

    // ----- Look for path to remove format (should end by /)
    if (($p_remove_path != '') && (substr($p_remove_path, -1) != '/'))
      $p_remove_path .= '/';
    $p_remove_path_size = strlen($p_remove_path);

    switch ($p_mode) {
      case "complete" :
        $v_extract_all = TRUE;
        $v_listing = FALSE;
      break;
      case "partial" :
          $v_extract_all = FALSE;
          $v_listing = FALSE;
      break;
      case "list" :
          $v_extract_all = FALSE;
          $v_listing = TRUE;
      break;
      default :
        $this->_error('Invalid extract mode ('.$p_mode.')');
        return false;
    }

    clearstatcache();

    While (!($v_end_of_file = ($this->_compress?@gzeof($this->_file):@feof($this->_file))))
    {
      $v_extract_file = FALSE;
      $v_extraction_stopped = 0;

      if ($this->_compress)
        $v_binary_data = @gzread($this->_file, 512);
      else
        $v_binary_data = @fread($this->_file, 512);

      if (!$this->_readHeader($v_binary_data, $v_header)) {
        fclose($this->_file);
        return false;
      }

      if ($v_header['filename'] == '')
        continue;

      if ((!$v_extract_all) && (is_array($p_file_list))) {
        // ----- By default no unzip if the file is not found
        $v_extract_file = false;

        for ($i=0; $i<sizeof($p_file_list); $i++) {
          // ----- Look if it is a directory
          if (substr($p_file_list[$i], -1) == '/') {
            // ----- Look if the directory is in the filename path
            if ((strlen($v_header['filename']) > strlen($p_file_list[$i])) && (substr($v_header['filename'], 0, strlen($p_file_list[$i])) == $p_file_list[$i])) {
              $v_extract_file = TRUE;
              break;
            }
          }

          // ----- It is a file, so compare the file names
          elseif ($p_file_list[$i] == $v_header['filename']) {
            $v_extract_file = TRUE;
            break;
          }
        }
      } else {
        $v_extract_file = TRUE;
      }

      // ----- Look if this file need to be extracted
      if (($v_extract_file) && (!$v_listing))
      {
        if (($p_remove_path != '')
            && (substr($v_header['filename'], 0, $p_remove_path_size) == $p_remove_path))
          $v_header['filename'] = substr($v_header['filename'], $p_remove_path_size);
        if (($p_path != './') && ($p_path != '/')) {
          while (substr($p_path, -1) == '/')
            $p_path = substr($p_path, 0, strlen($p_path)-1);

          if (substr($v_header['filename'], 0, 1) == '/')
              $v_header['filename'] = $p_path.$v_header['filename'];
          else
            $v_header['filename'] = $p_path.'/'.$v_header['filename'];
        }
        if (file_exists($v_header['filename'])) {
          if ((@is_dir($v_header['filename'])) && ($v_header['typeflag'] == '')) {
            $this->_error('File '.$v_header['filename'].' already exists as a directory');
            return false;
          }
          if ((is_file($v_header['filename'])) && ($v_header['typeflag'] == "5")) {
            $this->_error('Directory '.$v_header['filename'].' already exists as a file');
            return false;
          }
          if (!is_writeable($v_header['filename'])) {
            $this->_error('File '.$v_header['filename'].' already exists and is write protected');
            return false;
          }
          if (filemtime($v_header['filename']) > $v_header['mtime']) {
            // To be completed : An error or silent no replace ?
          }
        }

        // ----- Check the directory availability and create it if necessary
        elseif (($v_result = $this->_dirCheck(($v_header['typeflag'] == "5"?$v_header['filename']:dirname($v_header['filename'])))) != 1) {
            $this->_error('Unable to create path for '.$v_header['filename']);
            return false;
        }

        if ($v_extract_file) {
          if ($v_header['typeflag'] == "5") {
            if (!@file_exists($v_header['filename'])) {
                if (!@mkdir($v_header['filename'], 0777)) {
                    $this->_error('Unable to create directory {'.$v_header['filename'].'}');
                    return false;
                }
            }
          } else {
              if (($v_dest_file = @fopen($v_header['filename'], "wb")) == 0) {
                  $this->_error('Error while opening {'.$v_header['filename'].'} in write binary mode');
                  return false;
              } else {
                  $n = floor($v_header['size']/512);
                  for ($i=0; $i<$n; $i++) {
                      if ($this->_compress)
                          $v_content = @gzread($this->_file, 512);
                      else
                          $v_content = @fread($this->_file, 512);
                      fwrite($v_dest_file, $v_content, 512);
                  }
            if (($v_header['size'] % 512) != 0) {
              if ($this->_compress)
                $v_content = @gzread($this->_file, 512);
              else
                $v_content = @fread($this->_file, 512);
              fwrite($v_dest_file, $v_content, ($v_header['size'] % 512));
            }

            @fclose($v_dest_file);

            // ----- Change the file mode, mtime
            @touch($v_header['filename'], $v_header['mtime']);
            // To be completed
            //chmod($v_header[filename], DecOct($v_header[mode]));
          }

          // ----- Check the file size
          clearstatcache();
          if (filesize($v_header['filename']) != $v_header['size']) {
              $this->_error('Extracted file '.$v_header['filename'].' does not have the correct file size \''.filesize($v_filename).'\' ('.$v_header['size'].' expected). Archive may be corrupted.');
              return false;
          }
          }
        } else {
          // ----- Jump to next file
          if ($this->_compress)
              @gzseek($this->_file, @gztell($this->_file)+(ceil(($v_header['size']/512))*512));
          else
              @fseek($this->_file, @ftell($this->_file)+(ceil(($v_header['size']/512))*512));
        }
      } else {
        // ----- Jump to next file
        if ($this->_compress)
          @gzseek($this->_file, @gztell($this->_file)+(ceil(($v_header['size']/512))*512));
        else
          @fseek($this->_file, @ftell($this->_file)+(ceil(($v_header['size']/512))*512));
      }

      if ($this->_compress)
        $v_end_of_file = @gzeof($this->_file);
      else
        $v_end_of_file = @feof($this->_file);

      if ($v_listing || $v_extract_file || $v_extraction_stopped) {
        // ----- Log extracted files
        if (($v_file_dir = dirname($v_header['filename'])) == $v_header['filename'])
          $v_file_dir = '';
        if ((substr($v_header['filename'], 0, 1) == '/') && ($v_file_dir == ''))
          $v_file_dir = '/';

        $p_list_detail[$v_nb++] = $v_header;
      }
    }

        return true;
    }
    // }}}

    // {{{ _append()
    function _append($p_filelist, $p_add_dir='', $p_remove_dir='')
    {
        if ($this->_compress) {
            $this->_close();

            if (!@rename($this->_tarname, $this->_tarname.".tmp")) {
                $this->_error('Error while renaming \''.$this->_tarname.'\' to temporary file \''.$this->_tarname.'.tmp\'');
                return false;
            }

            if (($v_temp_tar = @gzopen($this->_tarname.".tmp", "rb")) == 0) {
                $this->_error('Unable to open file \''.$this->_tarname.'.tmp\' in binary read mode');
                @rename($this->_tarname.".tmp", $this->_tarname);
                return false;
            }

            if (!$this->_openWrite()) {
                @rename($this->_tarname.".tmp", $this->_tarname);
                return false;
            }

            $v_buffer = @gzread($v_temp_tar, 512);

            // ----- Read the following blocks but not the last one
            if (!@gzeof($v_temp_tar)) {
                do{
                    $v_binary_data = pack("a512", "$v_buffer");
                    @gzputs($this->_file, $v_binary_data);
                    $v_buffer = @gzread($v_temp_tar, 512);

                } while (!@gzeof($v_temp_tar));
            }

            if ($this->_addList($p_filelist, $p_add_dir, $p_remove_dir))
                $this->_writeFooter();

            $this->_close();
            @gzclose($v_temp_tar);

            if (!@unlink($this->_tarname.".tmp")) {
                $this->_error('Error while deleting temporary file \''.$this->_tarname.'.tmp\'');
            }

            return true;
        }

        // ----- For not compressed tar, just add files before the last 512 bytes block
        if (!$this->_openReadWrite())
           return false;

        clearstatcache();
        $v_size = filesize($this->_tarname);
        fseek($this->_file, $v_size-512);

        if ($this->_addList($p_filelist, $p_add_dir, $p_remove_dir))
           $this->_writeFooter();

        $this->_close();

        return true;
    }
    // }}}

    // {{{ _dirCheck()

    /**
     * Check if a directory exists and create it (including parent
     * dirs) if not.
     *
     * @param string $p_dir directory to check
     *
     * @return bool TRUE if the directory exists or was created
     */
    function _dirCheck($p_dir)
    {
        if ((@is_dir($p_dir)) || ($p_dir == ''))
            return true;

        $p_parent_dir = dirname($p_dir);

        if (($p_parent_dir != $p_dir) &&
            ($p_parent_dir != '') &&
            (!$this->_dirCheck($p_parent_dir)))
             return false;

        if (!@mkdir($p_dir, 0777)) {
            $this->_error("Unable to create directory '$p_dir'");
            return false;
        }

        return true;
    }

    // }}}

    // {{{ _pathReduction()

    /**
     * Compress path by changing for example "/dir/foo/../bar" to "/dir/bar", and
     * remove double slashes.
     *
     * @param string $p_dir path to reduce
     *
     * @return string reduced path
     *
     * @access private
     *
     */
    function _pathReduction($p_dir)
    {
        $v_result = '';

        // ----- Look for not empty path
        if ($p_dir != '') {
            // ----- Explode path by directory names
            $v_list = explode('/', $p_dir);

            // ----- Study directories from last to first
            for ($i=sizeof($v_list)-1; $i>=0; $i--) {
                // ----- Look for current path
                if ($v_list[$i] == ".") {
                    // ----- Ignore this directory
                    // Should be the first $i=0, but no check is done
                }
                else if ($v_list[$i] == "..") {
                    // ----- Ignore it and ignore the $i-1
                    $i--;
                }
                else if (($v_list[$i] == '') && ($i!=(sizeof($v_list)-1)) && ($i!=0)) {
                    // ----- Ignore only the double '//' in path,
                    // but not the first and last /
                } else {
                    $v_result = $v_list[$i].($i!=(sizeof($v_list)-1)?'/'.$v_result:'');
                }
            }
        }
        $v_result = strtr($v_result, '\\', '/');
        return $v_result;
    }

    // }}}

    // {{{ _translateWinPath()
    function _translateWinPath($p_path, $p_remove_disk_letter=true)
    {
      if (OS_WINDOWS) {
          // ----- Look for potential disk letter
          if (($p_remove_disk_letter) && (($v_position = strpos($p_path, ':')) != false)) {
              $p_path = substr($p_path, $v_position+1);
          }
          // ----- Change potential windows directory separator
          if ((strpos($p_path, '\\') > 0) || (substr($p_path, 0,1) == '\\')) {
              $p_path = strtr($p_path, '\\', '/');
          }
      }
      return $p_path;
    }
    // }}}

}
?>