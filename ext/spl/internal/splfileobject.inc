<?php

/** @file splfileobject.inc
 * @ingroup SPL
 * @brief class FileObject
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief   Object representation for any stream
 * @author  Marcus Boerger
 * @version 1.1
 * @since PHP 5.1
 */
class SplFileObject extends SplFileInfo implements RecursiveIterator, SeekableIterator
{
	/** Flag: wheter to suppress new lines */
	const DROP_NEW_LINE   = 0x00000001;

	private $fp;
	private $fname;
	private $line     = NULL;
	private $lnum     = 0;
	private $max_len  = 0;
	private $flags    = 0;
	private $delimiter= ',';
	private $enclosure= '"';
	
	/**
	 * Constructs a new file object
	 * 
	 * @param $file_name         The name of the stream to open
	 * @param $open_mode         The file open mode
	 * @param $use_include_path  Whether to search in include paths
	 * @param $context           A stream context
	 * @throw RuntimeException   If file cannot be opened (e.g. insufficient 
	 *                           access rights).
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
	 * @return whether the end of the stream is reached
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
		$this->freeLine();
		$this->lnum++;
		$buf = fgets($this->fp, $this->max_len);
		
		return $buf;
	}

	/**
	 * @param delimiter  character used as field separator
	 * @param enclosure  end of 
	 * @return array containing read data
	 */
	function fgetcsv($delimiter = NULL, $enclosure = NULL)
	{
		$this->freeLine();
		$this->lnum++;
		switch(fun_num_args())
		{
			case 0:
				$delimiter = $this->delimiter;
			case 1:
				$enclosure = $this->enclosure;
			default:
			case 2:
				break;
		}
		return fgetcsv($this->fp, $this->max_len, $delimiter, $enclosure); 
	}

	/**
	 * Set the delimiter and enclosure character used in fgetcsv
	 *
	 * @param delimiter new delimiter, defaults to ','
	 * @param enclosure new enclosure, defaults to '"'
	 */
	function setCsvControl($delimiter = ';', $enclosure = '"')
	{
		$this->delimiter = $delimiter;
		$this->enclosure = $enclosure;
	}

	/**
	 * @return array(delimiter, enclosure) as used in fgetcsv
	 */
	function getCsvControl($delimiter = ',', $enclosure = '"')
	{
		return array($this->delimiter, $this->enclosure);
	}

	/**
	 * @param operation lock operation (LOCK_SH, LOCK_EX, LOCK_UN, LOCK_NB)
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
	 * @param pos new file position
	 * @param whence seek method (SEEK_SET, SEEK_CUR, SEEK_END)
	 * @return Upon success, returns 0; otherwise, returns -1. Note that 
	 *         seeking past EOF is not considered an error.
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
		$this->freeLine();
		$c = fgetc($this->fp);
		if ($c == '\n') {
			$this->lnum++;
		}
	}

	/** Read and return remaining part of stream
	 * @return size of remaining part passed through
	 */
	function fpassthru()
	{
		return fpassthru($this->fp);
	}

	/** Get a line from the file and strip HTML tags
	 * @param $allowable_tags tags to keep in the string
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
		$this->freeLine();
		$this->lnum++;
		return fscanf($this->fp, $format /* , ... */);
	}

	/**
	 * @param $str to write
	 * @param $length maximum line length to write
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
	 * @param $size new size to truncate file to
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
	 * @return current setting for max line
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
		$this->freeLine();
		$this->lnum = 0;
	}

	/**
	 * @return whether more data can be read
	 */
	function valid()
	{
		return !$this->eof();
	}
	
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
	 * @return line number 
	 * @note fgetc() will increase the line number when reaing a new line char.
	 *       This has the effect key() called on a read a new line will already
	 *       return the increased line number.
	 * @note Line counting works as long as you only read the file and do not
	 *       use fseek().
	 */	
	function key()
	{
		return $this->lnum;
	}

	/** Invalidate current line buffer.
	 */	
	function next()
	{
		$this->freeLine();
	}

	/**
	 * @return next line read from file and increase the line counter
	 */
	private function readLine()
	{
		if ($this->eof())
		{
			$this->freeLine();
			throw new RuntimeException("Cannot read from file " . $this->fname);
		}
		if ($this->line) {
			$this->lnum++;
		}
		$this->freeLine();
		$this->line = fgets($this->fp, $this->max_len);
		return $this->line;
	}

	/**
	 * Free the current line buffer and increment the line counter
	 */
	private function freeLine()
	{
		if ($this->line) {
			$this->line = NULL;
		}
	}

	/*
	 * @note If you DO overload this function key() and current() will increment
	 *       $this->lnum automatically. If not then function reaLine() will do
	 *       that for you.
	 */ 
	function getCurrentLine()
	{
		$this->freeLine();
		if ($this->eof())
		{
			throw new RuntimeException("Cannot read from file " . $this->fname);
		}
		$this->readLine();
	}

	/**
	 * @return current line
	 */
	function __toString()
	{
		return current();
	}

	/**
	 * @param $line_pos Seek to this line
	 */	
	function seek($line_pos)
	{
		$this->rewind();
		while($this->lnum < $line_pos && !$this->eof())
		{
			$this->getCurrentLine();
		}
	}
}

?>
