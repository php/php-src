<?php
/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2002 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Stig Sæther Bakken <ssb@fast.no>                             |
  +----------------------------------------------------------------------+

  $Id$
*/

require_once "PEAR.php";

class PEAR_Frontend_CLI extends PEAR
{
    // {{{ properties

    /**
     * What type of user interface this frontend is for.
     * @var string
     * @access public
     */
    var $type = 'CLI';
    var $lp = ''; // line prefix

    var $omode = 'plain';
    var $params = array();
    var $term = array(
        'bold' => '',
        'normal' => '',
        );

    // }}}

    // {{{ constructor

    function PEAR_Frontend_CLI()
    {
        parent::PEAR();
        $term = getenv('TERM'); //(cox) $_ENV is empty for me in 4.1.1
        if ($term) {
            // XXX can use ncurses extension here, if available
            if (preg_match('/^(xterm|vt220|linux)/', $term)) {
                $this->term['bold'] = sprintf("%c%c%c%c", 27, 91, 49, 109);
                $this->term['normal']=sprintf("%c%c%c", 27, 91, 109);
            } elseif (preg_match('/^vt100/', $term)) {
                $this->term['bold'] = sprintf("%c%c%c%c%c%c", 27, 91, 49, 109, 0, 0);
                $this->term['normal']=sprintf("%c%c%c%c%c", 27, 91, 109, 0, 0);
            }
        } elseif (OS_WINDOWS) {
            // XXX add ANSI codes here
        }
    }

    // }}}

    // {{{ displayLine(text)

    function displayLine($text)
    {
        trigger_error("Frontend::displayLine deprecated", E_USER_ERROR);
    }

    function _displayLine($text)
    {
        print "$this->lp$text\n";
    }

    // }}}
    // {{{ display(text)

    function display($text)
    {
        trigger_error("Frontend::display deprecated", E_USER_ERROR);
    }

    function _display($text)
    {
        print $text;
    }

    // }}}
    // {{{ displayError(eobj)

    function displayError($eobj)
    {
        return $this->_displayLine($eobj->getMessage());
    }

    // }}}
    // {{{ displayFatalError(eobj)

    function displayFatalError($eobj)
    {
        $this->displayError($eobj);
        exit(1);
    }

    // }}}
    // {{{ displayHeading(title)

    function displayHeading($title)
    {
        trigger_error("Frontend::displayHeading deprecated", E_USER_ERROR);
    }

    function _displayHeading($title)
    {
        print $this->lp.$this->bold($title)."\n";
        print $this->lp.str_repeat("=", strlen($title))."\n";
    }

    // }}}
    // {{{ userDialog(prompt, [type], [default])

    function userDialog($command, $prompts, $types = array(), $defaults = array())
    {
        $result = array();
        if (is_array($prompts)) {
            $fp = fopen("php://stdin", "r");
            foreach ($prompts as $key => $prompt) {
                $type = $types[$key];
                $default = @$defaults[$key];
                if ($type == 'password') {
                    system('stty -echo');
                }
                print "$this->lp$prompt ";
                if ($default) {
                    print "[$default] ";
                }
                print ": ";
                $line = fgets($fp, 2048);
                if ($type == 'password') {
                    system('stty echo');
                    print "\n";
                }
                if ($default && trim($line) == "") {
                    $result[$key] = $default;
                } else {
                    $result[$key] = $line;
                }
            }
            fclose($fp);
        }
        return $result;
    }

    // }}}
    // {{{ userConfirm(prompt, [default])

    function userConfirm($prompt, $default = 'yes')
    {
        trigger_error("Frontend::userConfirm not yet converted", E_USER_ERROR);
        static $positives = array('y', 'yes', 'on', '1');
        static $negatives = array('n', 'no', 'off', '0');
        print "$this->lp$prompt [$default] : ";
        $fp = fopen("php://stdin", "r");
        $line = fgets($fp, 2048);
        fclose($fp);
        $answer = strtolower(trim($line));
        if (empty($answer)) {
            $answer = $default;
        }
        if (in_array($answer, $positives)) {
            return true;
        }
        if (in_array($answer, $negatives)) {
            return false;
        }
        if (in_array($default, $positives)) {
            return true;
        }
        return false;
    }

    // }}}
    // {{{ startTable([params])

    function startTable($params = array())
    {
        trigger_error("Frontend::startTable deprecated", E_USER_ERROR);
    }

    function _startTable($params = array())
    {
        $this->omode = 'table';
        $params['table_data'] = array();
        $params['widest'] = array();  // indexed by column
        $params['highest'] = array(); // indexed by row
        $params['ncols'] = 0;
        $this->params = $params;
    }

    // }}}
    // {{{ tableRow(columns, [rowparams], [colparams])

    function tableRow($columns, $rowparams = array(), $colparams = array())
    {
        trigger_error("Frontend::tableRow deprecated", E_USER_ERROR);
    }

    function _tableRow($columns, $rowparams = array(), $colparams = array())
    {
        $highest = 1;
        for ($i = 0; $i < sizeof($columns); $i++) {
            $col = &$columns[$i];
            if (isset($colparams[$i]) && !empty($colparams[$i]['wrap'])) {
                $col = wordwrap($col, $colparams[$i]['wrap'], "\n", 1);
            }
            if (strpos($col, "\n") !== false) {
                $multiline = explode("\n", $col);
                $w = 0;
                foreach ($multiline as $n => $line) {
                    if (strlen($line) > $w) {
                        $w = strlen($line);
                    }
                }
                $lines = sizeof($multiline);
            } else {
                $w = strlen($col);
            }
            if ($w > @$this->params['widest'][$i]) {
                $this->params['widest'][$i] = $w;
            }
            $tmp = count_chars($columns[$i], 1);
            // handle unix, mac and windows formats
            $lines = (isset($tmp[10]) ? $tmp[10] : @$tmp[13]) + 1;
            if ($lines > $highest) {
                $highest = $lines;
            }
        }
        if (sizeof($columns) > $this->params['ncols']) {
            $this->params['ncols'] = sizeof($columns);
        }
        $new_row = array(
            'data' => $columns,
            'height' => $highest,
            'rowparams' => $rowparams,
            'colparams' => $colparams,
            );
        $this->params['table_data'][] = $new_row;
    }

    // }}}
    // {{{ endTable()

    function endTable()
    {
        trigger_error("Frontend::tableRow deprecated", E_USER_ERROR);
    }

    function _endTable()
    {
        $this->omode = '';
        extract($this->params);
        if (!empty($caption)) {
            $this->_displayHeading($caption);
        }
        if (count($table_data) == 0) {
            return;
        }
        if (!isset($width)) {
            $width = $widest;
        } else {
            for ($i = 0; $i < $ncols; $i++) {
                if (!isset($width[$i])) {
                    $width[$i] = $widest[$i];
                }
            }
        }
        if (empty($border)) {
            $cellstart = '';
            $cellend = ' ';
            $rowend = '';
            $padrowend = false;
            $borderline = '';
        } else {
            $cellstart = '| ';
            $cellend = ' ';
            $rowend = '|';
            $padrowend = true;
            $borderline = '+';
            foreach ($width as $w) {
                $borderline .= str_repeat('-', $w + strlen($cellstart) + strlen($cellend) - 1);
                $borderline .= '+';
            }
        }
        if ($borderline) {
            $this->_displayLine($borderline);
        }
        for ($i = 0; $i < sizeof($table_data); $i++) {
            extract($table_data[$i]);
            $rowlines = array();
            if ($height > 1) {
                for ($c = 0; $c < sizeof($data); $c++) {
                    $rowlines[$c] = preg_split('/(\r?\n|\r)/', $data[$c]);
                    if (sizeof($rowlines[$c]) < $height) {
                        $rowlines[$c] = array_pad($rowlines[$c], $height, '');
                    }
                }
            } else {
                for ($c = 0; $c < sizeof($data); $c++) {
                    $rowlines[$c] = array($data[$c]);
                }
            }
            for ($r = 0; $r < $height; $r++) {
                $rowtext = '';
                for ($c = 0; $c < sizeof($data); $c++) {
                    if (isset($colparams[$c])) {
                        $attribs = array_merge($rowparams, $colparams);
                    } else {
                        $attribs = $rowparams;
                    }
                    $w = isset($width[$c]) ? $width[$c] : 0;
                    //$cell = $data[$c];
                    $cell = $rowlines[$c][$r];
                    $l = strlen($cell);
                    if ($l > $w) {
                        $cell = substr($cell, 0, $w);
                    }
                    if (isset($attribs['bold'])) {
                        $cell = $this->bold($cell);
                    }
                    if ($l < $w) {
                        // not using str_pad here because we may
                        // add bold escape characters to $cell
                        $cell .= str_repeat(' ', $w - $l);
                    }

                    $rowtext .= $cellstart . $cell . $cellend;
                }
                $rowtext .= $rowend;
                $this->_displayLine($rowtext);
            }
        }
        if ($borderline) {
            $this->_displayLine($borderline);
        }
    }

    // }}}
    // {{{ outputData()

    function outputData($data, $command = '_default')
    {
        switch ($command)
        {
            case 'install':
            case 'upgrade':
                if (isset($data['release_warnings'])) {
                    $this->_displayLine('');
                    $this->_startTable(array(
                        'border' => true,
                        'caption' => 'Release Warnings'
                        ));
                    $this->_tableRow(array($data['release_warnings']), null, array(1 => array('wrap' => 55)));
                    $this->_endTable();
                    $this->_displayLine('');
                };
                $this->_displayLine($data['data']);
                break;
            case 'search':
                $this->_startTable($data);
                if (isset($data['headline']) && is_array($data['headline']))
                    $this->_tableRow($data['headline'], array('bold' => true), array(1 => array('wrap' => 55)));

                foreach($data['data'] as $category) {
                    foreach($category as $pkg) {
                        $this->_tableRow($pkg, null, array(1 => array('wrap' => 55)));
                    }
                };
                $this->_endTable();
                break;
            case 'list-all':
                $this->_startTable($data);
                if (isset($data['headline']) && is_array($data['headline']))
                    $this->_tableRow($data['headline'], array('bold' => true), array(1 => array('wrap' => 55)));

                foreach($data['data'] as $category) {
                    foreach($category as $pkg) {
                        unset($pkg[3]);
                        unset($pkg[4]);
                        $this->_tableRow($pkg, null, array(1 => array('wrap' => 55)));
                    }
                };
                $this->_endTable();
                break;
            case 'config-show':
                $data['border'] = true;
                $opts = array(0 => array('wrap' => 20),
                              1 => array('wrap' => 20),
                              2 => array('wrap' => 35));
                $this->_startTable($data);
                if (isset($data['headline']) && is_array($data['headline'])) {
                    $this->_tableRow($data['headline'],
                                     array('bold' => true),
                                     $opts);
                }
                foreach($data['data'] as $group) {
                    foreach($group as $value) {
                        if ($value[2] == '') {
                            $value[2] = "<not set>";
                        }
                        $this->_tableRow($value, null, $opts);
                    }
                }
                $this->_endTable();
                break;
            case 'remote-info':
                $data = array(
                    'caption' => 'Package details:',
                    'border' => true,
                    'data' => array(
                        array("Latest",    $data['stable']),
                        array("Installed", $data['installed']),
                        array("Package",   $data['name']),
                        array("License",   $data['license']),
                        array("Category",  $data['category']),
                        array("Summary",   $data['summary']),
                        array("Description", $data['description']),
                        ),
                    );
            default: {
                if (is_array($data))
                {
                    $this->_startTable($data);
                    $count = count($data['data'][0]);
                    if ($count == 2) {
                        $opts = array(0 => array('wrap' => 25),
                                      1 => array('wrap' => 48)
                        );
                    } elseif ($count == 3) {
                        $opts = array(0 => array('wrap' => 20),
                                      1 => array('wrap' => 20),
                                      2 => array('wrap' => 35)
                        );
                    }
                    if (isset($data['headline']) && is_array($data['headline'])) {
                        $this->_tableRow($data['headline'],
                                         array('bold' => true),
                                         $opts);
                    }
                    foreach($data['data'] as $row) {
                        $this->_tableRow($row, null, $opts);
                    }
                    $this->_endTable();
                } else {
                    $this->_displayLine($data);
                }
            }
        }
    }

    // }}}
    // {{{ log(text)


    function log($text)
    {
        return $this->_displayLine($text);
    }


    // }}}
    // {{{ bold($text)

    function bold($text)
    {
        if (empty($this->term['bold'])) {
            return strtoupper($text);
        }
        return $this->term['bold'] . $text . $this->term['normal'];
    }

    // }}}
}

?>
