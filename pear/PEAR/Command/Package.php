<?php

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Packager.php';

class PEAR_Command_Package extends PEAR_Command_Common
{
    // {{{ constructor

    /**
     * PEAR_Command_Package constructor.
     *
     * @access public
     */
    function PEAR_Command_Package(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    // {{{ getCommands()

    /**
     * Return a list of all the commands defined by this class.
     * @return array list of commands
     * @access public
     */
    function getCommands()
    {
        return array('package', 'package-list', 'package-info');
    }

    // }}}
    // {{{ run()

    /**
     * Execute the command.
     *
     * @param string command name
     *
     * @param array option_name => value
     *
     * @param array list of additional parameters
     *
     * @return bool TRUE on success, FALSE for unknown commands, or
     * a PEAR error on failure
     *
     * @access public
     */
    function run($command, $options, $params)
    {
        $failmsg = '';
        switch ($command) {
            // {{{ package

            case 'package': {
                $pkginfofile = isset($params[0]) ? $params[0] : null;
                ob_start();
                $packager =& new PEAR_Packager($this->config->get('php_dir'),
                                               $this->config->get('ext_dir'),
                                               $this->config->get('doc_dir'));
                $packager->debug = $this->config->get('verbose');
                $result = $packager->Package($pkginfofile);
                $output = ob_get_contents();
                ob_end_clean();
                $lines = explode("\n", $output);
                foreach ($lines as $line) {
                    $this->ui->displayLine($line);
                }
                if (PEAR::isError($result)) {
                    $this->ui->displayLine("Package failed!");
                } else {
                    $this->ui->displayLine("Package ok.");
                }
                break;
            }

            // }}}
            // {{{ package-list

            case 'package-list': {
                include_once "PEAR/Common.php";
                $obj = new PEAR_Common();
                if (PEAR::isError($info = $obj->infoFromTgzFile($params[0]))) {
                    return $info;
                }
                $list =$info['filelist'];
                $caption = 'Contents of ' . basename($params[0]);
                $this->ui->startTable(array('caption' => $caption,
                                            'border' => true));
                $this->ui->tableRow(array('Package Files', 'Install Destination'),
                                    array('bold' => true));
                foreach ($list as $file => $att) {
                    if (isset($att['baseinstalldir'])) {
                        $dest = $att['baseinstalldir'] . DIRECTORY_SEPARATOR .
                                basename($file);
                    } else {
                        $dest = basename($file);
                    }
                    switch ($att['role']) {
                        case 'test':
                            $dest = '-- will not be installed --'; break;
                        case 'doc':
                            $dest = $this->config->get('doc_dir') . DIRECTORY_SEPARATOR .
                                    $dest;
                            break;
                        case 'php':
                        default:
                            $dest = $this->config->get('php_dir') . DIRECTORY_SEPARATOR .
                                    $dest;
                    }
                    $dest = preg_replace('!/+!', '/', $dest);
                    $file = preg_replace('!/+!', '/', $file);
                    $opts = array(0 => array('wrap' => 23),
                                  1 => array('wrap' => 45)
                                 );
                    $this->ui->tableRow(array($file, $dest), null, $opts);
                }
                $this->ui->endTable();
                break;
            }

            // }}}
            // {{{ package-info

            case 'package-info': {
                include_once "PEAR/Common.php";
                $obj = new PEAR_Common();
                if (PEAR::isError($info = $obj->infoFromTgzFile($params[0]))) {
                    return $info;
                }
                unset($info['filelist']);
                unset($info['changelog']);
                $keys = array_keys($info);
                $longtext = array('description', 'summary');
                foreach ($keys as $key) {
                    if (is_array($info[$key])) {
                        switch ($key) {
                            case 'maintainers': {
                                $i = 0;
                                $mstr = '';
                                foreach ($info[$key] as $m) {
                                    if ($i++ > 0) {
                                        $mstr .= "\n";
                                    }
                                    $mstr .= $m['name'] . " <";
                                    if (isset($m['email'])) {
                                        $mstr .= $m['email'];
                                    } else {
                                        $mstr .= $m['handle'] . '@php.net';
                                    }
                                    $mstr .= "> ($m[role])";
                                }
                                $info[$key] = $mstr;
                                break;
                            }
                            case 'release_deps': {
                                static $rel_trans = array(
                                    'lt' => '<',
                                    'le' => '<=',
                                    'eq' => '=',
                                    'ne' => '!=',
                                    'gt' => '>',
                                    'ge' => '>=',
                                    );
                                $i = 0;
                                $dstr = '';
                                foreach ($info[$key] as $d) {
                                    if ($i++ > 0) {
                                        $dstr .= ", ";
                                    }
                                    if (isset($rel_trans[$d['rel']])) {
                                        $d['rel'] = $rel_trans[$d['rel']];
                                    }
                                    $dstr .= "$d[type] $d[rel]";
                                    if (isset($d['version'])) {
                                        $dstr .= " $d[version]";
                                    }
                                }
                                $info[$key] = $dstr;
                                break;
                            }
                            default: {
                                $info[$key] = implode(", ", $info[$key]);
                                break;
                            }
                        }
                    }
                    $info[$key] = trim($info[$key]);
                    if (in_array($key, $longtext)) {
                        $info[$key] = preg_replace('/  +/', ' ', $info[$key]);
                    }
                }
                $caption = 'About ' . basename($params[0]);
                $this->ui->startTable(array('caption' => $caption,
                                            'border' => true));
                foreach ($info as $key => $value) {
                    $key = ucwords(str_replace('_', ' ', $key));
                    $this->ui->tableRow(array($key, $value), null, array(1 => array('wrap' => 55)));
                }
                $this->ui->endTable();
                break;
            }

            // }}}
            default: {
                return false;
            }
        }
        if ($failmsg) {
            return $this->raiseError($failmsg);
        }
        return true;
    }

    // }}}


}

?>