<?php
// this corrupt zip maker uses portions of Vincent Lascaux's File_Archive to create
// specifically corrupted zip archives for unit-testing zip support in the phar extension
// and was modified by Greg Beaver
/**
 * ZIP archive writer
 *
 * PHP versions 4 and 5
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,Boston,MA 02111-1307 USA
 *
 * @category   File Formats
 * @package    File_Archive
 * @author     Vincent Lascaux <vincentlascaux@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.gnu.org/copyleft/lesser.html  LGPL
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/File_Archive
 */

/**
 * ZIP archive writer
 */
class corrupt_zipmaker
{
	/**
	* @var int Current position in the writer
	* @access private
	*/
	var $offset = 0;

	/**
	* @var string Optionnal comment to add to the zip
	* @access private
	*/
	var $comment = "";

	/**
	* @var string Data written at the end of the ZIP file
	* @access private
	*/
	var $central = "";

	/**
	* @var string Data written at the start of the ZIP file
	* @access private
	*/
	var $start = "";

	/**
	* Set a comment on the ZIP file
	*/
	function setComment($comment) { $this->comment = $comment; }

	/**
	* @param int $time Unix timestamp of the date to convert
	* @return the date formated as a ZIP date
	*/
	function getMTime($time)
	{
		$mtime = ($time !== null ? getdate($time) : getdate());
		$mtime = preg_replace(
				"/(..){1}(..){1}(..){1}(..){1}/",
				"\\x\\4\\x\\3\\x\\2\\x\\1",
				dechex(($mtime['year']-1980<<25)|
					($mtime['mon'    ]<<21)|
					($mtime['mday'   ]<<16)|
					($mtime['hours'  ]<<11)|
					($mtime['minutes']<<5)|
					($mtime['seconds']>>1)));
		eval('$mtime = "'.$mtime.'";');
		return $mtime;
	}

	private function getFileEntry($compmethod, $mtime, $crc32, $complength, $uncomplength, $filename, $data, $corrupt, $fakecomp)
	{
		switch ($corrupt) {
			case null :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32, $complength, $uncomplength, strlen($filename), 0x00) .
						$filename .
						$data;
				break;
			case 'compress' :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $fakecomp) .
						$mtime .
						pack("VVVvv", $crc32, $complength, $uncomplength, strlen($filename), 0x00) .
						$filename .
						$data;
				break;
			case 'encrypt' :
				$file = "PK\x03\x04\x14\x00\x01\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32, $complength, $uncomplength, strlen($filename), 0x00) .
						$filename .
						$data;
				break;
			case 'crc32' :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32 + 1, $complength, $uncomplength, strlen($filename), 0x00) .
						$filename .
						$data;
				break;
			case 'complength' :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32, $complength + 1, $uncomplength, strlen($filename), 0x00) .
						$filename .
						$data;
				break;
			case 'uncomplength' :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32, $complength, $uncomplength - 1, strlen($filename), 0x00) .
						$filename .
						$data;
				break;
			case 'filename_len' :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32, $complength, $uncomplength, strlen($filename) - 1, 0x00) .
						$filename .
						$data;
				break;
			case 'extra_len' :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32, $complength, $uncomplength, strlen($filename), 1) .
						$filename .
						$data;
				break;
			case 'filename' :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32, $complength, $uncomplength, strlen($filename), 0x00) .
						substr($filename, 1) .
						$data;
				break;
			case 'data' :
				$file = "PK\x03\x04\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvv", $crc32, $complength, $uncomplength, strlen($filename), 0x00) .
						$filename .
						substr($data, 1);
				break;
		}
		return $file;
	}

	private function getCentralEntry($compmethod, $mtime, $crc32, $complength, $uncomplength, $filename, $comment, $corrupt, &$offset, $fakecomp)
	{
		settype($comment, 'string');
		switch ($corrupt) {
			case null :
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength, $uncomplength, strlen($filename), 0x00,strlen($comment),0x00,0x00,
							0x0000, $this->offset).
						$filename . $comment;
				$offset = strlen($central);
				break;
			case 'encrypt' :
				$central = "PK\x01\x02\x00\x00\x14\x00\x01\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength, $uncomplength, strlen($filename), 0x00,strlen($comment),0x00,0x00,
							0x0000, $this->offset).
						$filename . $comment;
				$offset = strlen($central);
				break;
			case 'compress' :
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $fakecomp) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength, $uncomplength, strlen($filename), 0x00,strlen($comment),0x00,0x00,
							0x0000, $this->offset).
						$filename . $comment;
				$offset = strlen($central);
				break;
			case 'crc32' :
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32 + 1, $complength, $uncomplength, strlen($filename), 0x00,strlen($comment),0x00,0x00,
							0x0000, $this->offset).
						$filename . $comment;
				$offset = strlen($central);
				break;
			case 'complength' :
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength - 1, $uncomplength, strlen($filename), 0x00,strlen($comment),0x00,0x00,
							0x0000, $this->offset).
						$filename . $comment;
				$offset = strlen($central);
				break;
			case 'uncomplength' :
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength, $uncomplength - 1, strlen($filename), 0x00,strlen($comment),0x00,0x00,
							0x0000, $this->offset).
						$filename . $comment;
				$offset = strlen($central);
				break;
			case 'filename_len' :
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength, $uncomplength, strlen($filename) - 1, 0x00,strlen($comment),0x00,0x00,
							0x0000, $this->offset).
						$filename . $comment;
				$offset = strlen($central);
				break;
			case 'offset' :
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength, $uncomplength, strlen($filename), 0x00,strlen($comment),0x00,0x00,
							0x0000, $this->offset - 1).
						$filename . $comment;
				$offset = strlen($central) - 1;
				break;
			case 'comment' :
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength, $uncomplength, strlen($filename), 0x00,strlen($comment) + 1,0x00,0x00,
							0x0000, $this->offset).
						$filename . $comment;
				$offset = strlen($central);
				break;
			case 'extralen1' :
				$extra = 'nu' . 0xffff; // way huge size
				$central = "PK\x01\x02\x00\x00\x14\x00\x00\x00" . pack('v', $compmethod) .
						$mtime .
						pack("VVVvvvvvVV", $crc32, $complength, $uncomplength, strlen($filename), strlen($extra),strlen($comment),0x00,0x00,
							0x0000, $this->offset).
						$filename . $extra . $comment;
				$offset = strlen($central);
				break;
		}
		return $central;
	}

	function addFile($filename, $mtime, $data, $comment = null, $compress = null, $filecorrupt = null, $centralcorrupt = null, $fakecomp = 1)
	{
		$mtime = $this->getMTime($mtime ? $mtime : null);

		$uncomplength = strlen($data);
		$crc32 = crc32($data) & 0xFFFFFFFF;
		$compmethod = 0;
		switch ($compress) {
			case 'gz' :
				$data = gzcompress($data);
				$compmethod = 8;
				break;
			case 'bz2' :
				$data = bzcompress($data);
				$compmethod = 12;
				break;
		}
		$complength = strlen($data);

		$this->start .= ($file = $this->getFileEntry($compmethod, $mtime, $crc32, $complength, $uncomplength, $filename, $data, $filecorrupt, $fakecomp));

		$offset = 0;
		$this->central .= $this->getCentralEntry($compmethod, $mtime, $crc32, $complength, $uncomplength, $filename, $comment, $centralcorrupt, $offset, $fakecomp);

		$this->offset += $offset;
		$this->count++;
	}

	function writeZip($zipfile, $corrupt = null)
	{
		$write = $this->start . $this->central;
		switch ($corrupt) {
			case null :
				$write .= "PK\x05\x06\x00\x00\x00\x00" .
					pack("vvVVv", $this->count, $this->count,
					$this->offset, strlen($this->start),
					strlen($this->comment)) . $this->comment;
				break;
			case 'disknumber' :
				$write .= "PK\x05\x06\x01\x00\x01\x00" .
					pack("vvVVv", $this->count, $this->count,
					$this->offset, strlen($this->start),
					strlen($this->comment)) . $this->comment;
				break;
			case 'count1' :
				$write .= "PK\x05\x06\x00\x00\x00\x00" .
					pack("vvVVv", $this->count + 1, $this->count,
					$this->offset, strlen($this->start),
					strlen($this->comment)) . $this->comment;
				break;
			case 'count2' :
				$write .= "PK\x05\x06\x00\x00\x00\x00" .
					pack("vvVVv", $this->count, $this->count + 1,
					$this->offset, strlen($this->start),
					strlen($this->comment)) . $this->comment;
				break;
			case 'cdir_offset' :
				$write .= "PK\x05\x06\x00\x00\x00\x00" .
					pack("vvVVv", $this->count, $this->count,
					$this->offset, strlen($this->start) - 3,
					strlen($this->comment)) . $this->comment;
				break;
			case 'cdir_len' :
				$write .= "PK\x05\x06\x00\x00\x00\x00" .
					pack("vvVVv", $this->count, $this->count,
					$this->offset - 5, strlen($this->start),
					strlen($this->comment)) . $this->comment;
				break;
			case 'comment' :
				$write .= "PK\x05\x06\x00\x00\x00\x00" .
					pack("vvVVv", $this->count, $this->count,
					strlen($this->start), $this->offset + 1,
					strlen($this->comment) + 1) . $this->comment;
				break;
			case 'none' :
		}
		file_put_contents($zipfile, $write);
	}
}
?>