<?php

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Remote.php';

class PEAR_Command_Remote extends PEAR_Command_Common
{
    // {{{ constructor

    /**
     * PEAR_Command_Remote constructor.
     *
     * @access public
     */
    function PEAR_Command_Remote(&$ui, &$config)
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
        return array('remote-package-info',
                     'list-upgrades',
                     'list-remote-packages',
                     'download');
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
        $remote = &new PEAR_Remote($this->config);
        switch ($command) {
            case 'remote-package-info': {
                break;
            }
            case 'list-remote-packages': {
                break;
            }
            case 'download': {
                //$params[0] -> The package to download
                if (count($params) != 1) {
                    return PEAR::raiseError("download expects one argument: the package to download");
                }
                if (!ereg('^http://', $params[0])) {
                    $pkgfile = "http://" . $this->config->get('master_server') .
                               "/get/" . $params[0];
                } else {
                    $pkgfile = $params[0];
                }
                if (!extension_loaded("zlib")) {
                    $pkgfile .= '?uncompress=yes';
                }
                include_once 'HTTP.php';
                $headers = HTTP::head($pkgfile);
                if (PEAR::isError($headers)|| !isset($headers['Content-disposition'])) {
                    return $this->raiseError("Could not retrieve remote file information");
                }
                preg_match('|filename="(.*)"$|', $headers['Content-disposition'], $matches);
                $fname = $matches[1];
                if (!$wp = @fopen($pkgfile, 'wb')) {
                    $failmsg = "Could not download $pkgfile"; break;
                }
                if (!$fp = @fopen($fname, 'wb')) {
                    $failmsg = "Could not write the file here"; break;
                }
                $bytes = 0;
                $this->ui->displayLine("Downloading $pkgfile:");
                while ($data = @fread($wp, 16384)) {
                    $bytes += strlen($data);
                    if (!@fwrite($fp, $data)) {
                        return $this->raiseError("$pkgfile: write failed ($php_errormsg)");
                    }
                    $this->ui->display('...');
                }
                $this->ui->display(" done!\n");
                fclose($fp);
                fclose($wp);
                $this->ui->displayLine("File $fname downloaded ($bytes bytes)");
                break;
            }
            case 'list-upgrades': {
                include_once "PEAR/Registry.php";
                if (empty($params[0])) {
                    $state = $this->config->get('preferred_state');
                } else {
                    $state = $params[0];
                }
                $caption = 'Available Upgrades';
                if (empty($state) || $state == 'any') {
                    $latest = $remote->call("package.listLatestReleases");
                } else {
                    $latest = $remote->call("package.listLatestReleases", $state);
                    $caption .= ' (' . $state . ')';
                }
                $caption .= ':';
                if (PEAR::isError($latest)) {
                    return $latest;
                }
                $reg = new PEAR_Registry($this->config->get('php_dir'));
                $inst = array_flip($reg->listPackages());
                $this->ui->startTable(array('caption' => $caption,
                                            'border' => 1));
                $this->ui->tableRow(array('Package', 'Version', 'Size'),
                                array('bold' => true));
                foreach ($latest as $package => $info) {
                    if (!isset($inst[$package])) {
                        // skip packages we don't have installed
                        continue;
                    }
                    extract($info);
                    $inst_version = $reg->packageInfo($package, 'version');
                    if (version_compare($version, $inst_version, "le")) {
                        // installed version is up-to-date
                        continue;
                    }
                    if ($filesize >= 20480) {
                        $filesize += 1024 - ($filesize % 1024);
                        $fs = sprintf("%dkB", $filesize / 1024);
                    } elseif ($filesize > 0) {
                        $filesize += 103 - ($filesize % 103);
                        $fs = sprintf("%.1fkB", $filesize / 1024.0);
                    } else {
                        $fs = "  -"; // XXX center instead
                    }
                    $this->ui->tableRow(array($package, $version, $fs));
                }
                $this->ui->endTable();
                break;
            }
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