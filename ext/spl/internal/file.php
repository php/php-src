<?php

/** @file file.inc
 * @ingroup SPL
 * @brief class File
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief   Object representation for any stream
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.1
 */
class File extends RecursiveIterator
{
	private $fp;
	private $fname;
	private $line     = NULL;
	private $lnum     = 0;
	private $max_len  = 0;
	private $flags    = 0;
	
	/**
	 * Constructs a new file object
	 * 
	 * @param $file_name         The name of the stream to open
	 * @param $open_mode         The fle open mode
	 * @param $use_include_path  Whether to search in include paths
	 * @param $context           A stream context
	 */
	function __construct($file_name, $open_mode = 'r', $use_include_path = false, $context = NULL)
	{
		$this->fp = fopen($file_name, $open_mode, $use_include_path, $context);
		if (!$this->fp)
		{
			throw new RuntimeException("Cannot open file $file_name");
		}
		$this->fname = $file_name;
	}
	
	/**
	 * @return the filename as specified in the constructor
	 */
	function getFilename()
	{
		return $this->fname;
	}

	/**
	 * @return whethe rend of stream is reached
	 */
	function eof()
	{
		return eof($this->fp);
	}

	/** increase current line number
	 * @return next line from stream
	 */
	function fgets()
	{
		$this->lnum++;
		$buf = fgets($this->fp, $this->max_len);
		
		return $buf;
	}

	/**
	 * @$delimiter  character used as field separator
	 * @enclosure   end of 
	 */
	function fgetcsv($delimiter = ';', $enclosure = '')
	{
		$this->lnum++;
		return fgetcsv($this->fp, $this->max_len, $delimiter, $enclosure); 
	}

	/**
	 * @param operation lock opeation (LOCK_SH, LOCK_EX, LOCK_UN, LOCK_NB)
	 * @retval $wouldblock  whether the operation would block
	 */
	function flock($operation, &$wouldblock)
	{
		return flock($this->fp, $operation, $wouldblock);
	}

	/**
	 * Flush current data
	 * @return success or failure
	 */
	function fflush()
	{
		return fflush($this->fp);
	}

	/**
	 * @return current file position
	 */
	function ftell()
	{
		return ftell($this->fp);
	}

	/**
	 * @pos new file position
	 * @whence seek method (SEEK_SET, SEEK_CUR, SEEK_END)
	 */
	function fseek($pos, $whence = SEEK_SET)
	{
		return fseek($this->fp, $pos, $whence);
	}

	/**
	 * @return next char from file
	 * @note a new line character does not increase $this->lnum
	 */
	function fgetc()
	{
		return fgetc($this->fp);
	}

	/** Read and return remaining part of stream
	 * @return size of remaining part passed through
	 */
	function fpassthru()
	{
		return fpassthru($this->fp);
	}

	/** Get a line from the file and strip HTML tags
	 * @param $allow_tags tags to keep in the string
	 */
	function fgetss($allowable_tags = NULL)
	{
		return fgetss($this->fp, $allowable_tags);
	}

	/** Scan the next line
	 * @param $format string specifying format to parse
	 */	
	function fscanf($format /* , ... */)
	{
		$this->lnum++;
		return fscanf($this->fp, $format /* , ... */);
	}

	/**
	 * @param $str to write
	 * @param $lngth maximum line length to write
	 */
	function fwrite($str, $length = NULL)
	{
		return fwrite($this->fp, $length);
	}

	/**
	 * @return array of file stat information
	 */
	function fstat()
	{
		return fstat($this->fp);
	}

	/**
	 * @param new size to truncate file to
	 */
	function ftruncate($size)
	{
		return ftruncate($this->fp, $size);
	}

	/**
	 * @param $flags new flag set
	 */
	function setFlags($flags)
	{
		$this->flags = $flags;
	}

	/**
	 *  @return current set of flags
	 */
	function getFlags()
	{
		return $this->flags;
	}

	/**
	 * @param $max_len set the maximum line length read
	 */
	function setMaxLineLen($max_len)
	{
		$this->max_len = $max_len;
	}

	/**
	 * @return curren tsetting for max line
	 */
	function getMaxLineLen()
	{
		return $this->max_len;
	}

	/**
	 * @return false
	 */
	function hasChildren()
	{
		return false;
	}

	/**
	 * @return false
	 */
	function getChildren()
	{
		return NULL;
	}

	/**
	 * Invalidate current line buffer and set line number to 0.
	 */
	function rewind()
	{
		$this->line = NULL;
		$this->lnum = 0;
	}

	/**
	 * @return whether more data can be read
	 */
	function valid()
	{
		return !$this->eof();
	
	/**
	 * @note Fill current line buffer if not done yet.
	 * @return line buffer 
	 */	
	function current()
	{
		if (is_null($this->line))
		{
			$this->line = getCurrentLine();
		}
		return $this->line;
	}

	/**
	 * @note Fill current line buffer if not done yet.
	 * @return line number 
	 */	
	function key()
	{
		if (is_null($this->line))
		{
			$line = getCurrentLine();
		}
		return $this->lnum;
	}

	/** Invalidate current line buffer.
	 */	
	function next()
	{
		$this->line = NULL;
	}

	/**
	 * @return next line read from file and increase the line counter
	 */
	private function readLine()
	{
		$this->lnum++;
		return fgets($this->fp, $this->max_len);
	}

	/*
	 * @note if you overload this function key() and current() will increment
	 *       $this->lnum.
	 */ 
	function getCurrentLine()
	{
		$this->readLine();
	}

	/**
	 * @return current line
	 */
	function __toString()
	{
		return current();
	}
}

?>
