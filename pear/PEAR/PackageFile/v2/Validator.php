<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 5                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2004 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Greg Beaver <cellog@php.net>                                 |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$
/**
 * Private validation class used by PEAR_PackageFile_v2 - do not use directly, its
 * sole purpose is to split up the PEAR/PackageFile/v2.php file to make it smaller
 * @author Greg Beaver <cellog@php.net>
 * @access private
 */
class PEAR_PackageFile_v2_Validator
{
    /**
     * @var array
     */
    var $_packageInfo;
    /**
     * @var PEAR_PackageFile_v2
     */
    var $_pf;
    /**
     * @var PEAR_ErrorStack
     */
    var $_stack;
    /**
     * @var int
     */
    var $_isValid = 0;
    /**
     * @var int
     */
    var $_curState = 0;
    /**
     * @param PEAR_PackageFile_v2
     * @param int
     */
    function validate(&$pf, $state = PEAR_VALIDATE_NORMAL)
    {
        $this->_pf = &$pf;
        $this->_curState = $state;
        $this->_packageInfo = $this->_pf->getArray();
        $this->_isValid = $this->_pf->_isValid;
        $this->_filesValid = $this->_pf->_filesValid;
        $this->_stack = &$pf->_stack;
        $this->_stack->getErrors(true);
        if (($this->_isValid & $state) == $state) {
            return true;
        }
        if (!isset($this->_packageInfo) || !is_array($this->_packageInfo)) {
            return false;
        }
        if (!isset($this->_packageInfo['attribs']['version']) ||
              $this->_packageInfo['attribs']['version'] != '2.0') {
            $this->_noPackageVersion();
        }
        $structure =
        array(
            'name',
            'channel|uri',
            '*extends', // can't be multiple, but this works fine
            'summary',
            'description',
            '+lead', // these all need content checks
            '*developer',
            '*contributor',
            '*helper',
            'date',
            '*time',
            'version',
            'stability',
            'license->?uri->?filesource',
            'notes',
            'contents', //special validation needed
            '*compatible',
            'dependencies', //special validation needed
            '*usesrole',
            '*usestask', // reserve these for 1.4.0a1 to implement
                         // this will allow a package.xml to gracefully say it
                         // needs a certain package installed in order to implement a role or task
            '*providesextension',
            '*srcpackage|*srcuri',
            '+phprelease|+extsrcrelease|+extbinrelease|bundle', //special validation needed
            '*changelog',
        );
        $test = $this->_packageInfo;
        // ignore post-installation array fields
        if (array_key_exists('filelist', $test)) {
            unset($test['filelist']);
        }
        if (array_key_exists('_lastmodified', $test)) {
            unset($test['_lastmodified']);
        }
        if (array_key_exists('#binarypackage', $test)) {
            unset($test['#binarypackage']);
        }
        if (array_key_exists('old', $test)) {
            unset($test['old']);
        }
        if (array_key_exists('_lastversion', $test)) {
            unset($test['_lastversion']);
        }
        if (!$this->_stupidSchemaValidate($structure,
                                          $test, '<package>')) {
            return false;
        }
        if (empty($this->_packageInfo['name'])) {
            $this->_tagCannotBeEmpty('name');
        }
        if (isset($this->_packageInfo['uri'])) {
            $test = 'uri';
        } else {
            $test = 'channel';
        }
        if (empty($this->_packageInfo[$test])) {
            $this->_tagCannotBeEmpty($test);
        }
        if (is_array($this->_packageInfo['license']) &&
              (!isset($this->_packageInfo['license']['_content']) ||
              empty($this->_packageInfo['license']['_content']))) {
            $this->_tagCannotBeEmpty('license');
        } elseif (empty($this->_packageInfo['license'])) {
            $this->_tagCannotBeEmpty('license');
        }
        if (empty($this->_packageInfo['summary'])) {
            $this->_tagCannotBeEmpty('summary');
        }
        if (empty($this->_packageInfo['description'])) {
            $this->_tagCannotBeEmpty('description');
        }
        if (empty($this->_packageInfo['date'])) {
            $this->_tagCannotBeEmpty('date');
        }
        if (empty($this->_packageInfo['notes'])) {
            $this->_tagCannotBeEmpty('notes');
        }
        if (isset($this->_packageInfo['time']) && empty($this->_packageInfo['time'])) {
            $this->_tagCannotBeEmpty('time');
        }
        if (isset($this->_packageInfo['dependencies'])) {
            $this->_validateDependencies();
        }
        if (isset($this->_packageInfo['compatible'])) {
            $this->_validateCompatible();
        }
        if (!isset($this->_packageInfo['bundle'])) {
            if (!isset($this->_packageInfo['contents']['dir'])) {
                $this->_filelistMustContainDir('contents');
                return false;
            }
            if (isset($this->_packageInfo['contents']['file'])) {
                $this->_filelistCannotContainFile('contents');
                return false;
            }
        }
        $this->_validateMaintainers();
        $this->_validateStabilityVersion();
        $fail = false;
        if (array_key_exists('usesrole', $this->_packageInfo)) {
            $roles = $this->_packageInfo['usesrole'];
            if (!is_array($roles) || !isset($roles[0])) {
                $roles = array($roles);
            }
            foreach ($roles as $role) {
                if (!isset($role['role'])) {
                    $this->_usesroletaskMustHaveRoleTask('usesrole', 'role');
                    $fail = true;
                } else {
                    if (!isset($role['channel'])) {
                        if (!isset($role['uri'])) {
                            $this->_usesroletaskMustHaveChannelOrUri($role['role'], 'usesrole');
                            $fail = true;
                        }
                    } elseif (!isset($role['package'])) {
                        $this->_usesroletaskMustHavePackage($role['role'], 'usesrole');
                        $fail = true;
                    }
                }
            }
        }
        if (array_key_exists('usestask', $this->_packageInfo)) {
            $roles = $this->_packageInfo['usestask'];
            if (!is_array($roles) || !isset($roles[0])) {
                $roles = array($roles);
            }
            foreach ($roles as $role) {
                if (!isset($role['task'])) {
                    $this->_usesroletaskMustHaveRoleTask('usestask', 'task');
                    $fail = true;
                } else {
                    if (!isset($role['channel'])) {
                        if (!isset($role['uri'])) {
                            $this->_usesroletaskMustHaveChannelOrUri($role['task'], 'usestask');
                            $fail = true;
                        }
                    } elseif (!isset($role['package'])) {
                        $this->_usesroletaskMustHavePackage($role['task'], 'usestask');
                        $fail = true;
                    }
                }
            }
        }
        if ($fail) {
            return false;
        }
        $this->_validateFilelist();
        $this->_validateRelease();
        if (!$this->_stack->hasErrors()) {
            $chan = $this->_pf->_registry->getChannel($this->_pf->getChannel(), true);
            if (!$chan) {
                $this->_unknownChannel($this->_pf->getChannel());
            } else {
                $valpack = $chan->getValidationPackage();
                // for channel validator packages, always use the default PEAR validator.
                // otherwise, they can't be installed or packaged
                $validator = $chan->getValidationObject($this->_pf->getPackage());
                if (!$validator) {
                    $this->_stack->push(__FUNCTION__, 'error',
                        array_merge(
                            array('channel' => $chan->getName(),
                                  'package' => $this->_pf->getPackage()),
                              $valpack
                            ),
                        'package "%channel%/%package%" cannot be properly validated without ' .
                        'validation package "%channel%/%name%-%version%"');
                    return $this->_isValid = 0;
                }
                $validator->setPackageFile($this->_pf);
                $validator->validate($state);
                $failures = $validator->getFailures();
                foreach ($failures['errors'] as $error) {
                    $this->_stack->push(__FUNCTION__, 'error', $error,
                        'Channel validator error: field "%field%" - %reason%');
                }
                foreach ($failures['warnings'] as $warning) {
                    $this->_stack->push(__FUNCTION__, 'warning', $warning,
                        'Channel validator warning: field "%field%" - %reason%');
                }
            }
        }
        $this->_pf->_isValid = $this->_isValid = !$this->_stack->hasErrors('error');
        if ($this->_isValid && $state == PEAR_VALIDATE_PACKAGING && !$this->_filesValid) {
            if ($this->_pf->getPackageType() == 'bundle') {
                if ($this->_analyzeBundledPackages()) {
                    $this->_filesValid = $this->_pf->_filesValid = true;
                } else {
                    $this->_pf->_isValid = $this->_isValid = 0;
                }
            } else {
                if (!$this->_analyzePhpFiles()) {
                    $this->_pf->_isValid = $this->_isValid = 0;
                } else {
                    $this->_filesValid = $this->_pf->_filesValid = true;
                }
            }
        }
        if ($this->_isValid) {
            return $this->_pf->_isValid = $this->_isValid = $state;
        }
        return $this->_pf->_isValid = $this->_isValid = 0;
    }

    function _stupidSchemaValidate($structure, $xml, $root)
    {
        if (!is_array($xml)) {
            $xml = array();
        }
        $keys = array_keys($xml);
        reset($keys);
        $key = current($keys);
        while ($key == 'attribs' || $key == '_contents') {
            $key = next($keys);
        }
        $unfoundtags = $optionaltags = array();
        $ret = true;
        $mismatch = false;
        foreach ($structure as $struc) {
            if ($key) {
                $tag = $xml[$key];
            }
            $test = $this->_processStructure($struc);
            if (isset($test['choices'])) {
                $loose = true;
                foreach ($test['choices'] as $choice) {
                    if ($key == $choice['tag']) {
                        $key = next($keys);
                        while ($key == 'attribs' || $key == '_contents') {
                            $key = next($keys);
                        }
                        $unfoundtags = $optionaltags = array();
                        $mismatch = false;
                        if ($key && $key != $choice['tag'] && isset($choice['multiple'])) {
                            $unfoundtags[] = $choice['tag'];
                            $optionaltags[] = $choice['tag'];
                            if ($key) {
                                $mismatch = true;
                            }
                        }
                        $ret &= $this->_processAttribs($choice, $tag, $root);
                        continue 2;
                    } else {
                        $unfoundtags[] = $choice['tag'];
                        $mismatch = true;
                    }
                    if (!isset($choice['multiple']) || $choice['multiple'] != '*') {
                        $loose = false;
                    } else {
                        $optionaltags[] = $choice['tag'];
                    }
                }
                if (!$loose) {
                    $this->_invalidTagOrder($unfoundtags, $key, $root);
                    return false;
                }
            } else {
                if ($key != $test['tag']) {
                    if (isset($test['multiple']) && $test['multiple'] != '*') {
                        $unfoundtags[] = $test['tag'];
                        $this->_invalidTagOrder($unfoundtags, $key, $root);
                        return false;
                    } else {
                        if ($key) {
                            $mismatch = true;
                        }
                        $unfoundtags[] = $test['tag'];
                        $optionaltags[] = $test['tag'];
                    }
                    if (!isset($test['multiple'])) {
                        $this->_invalidTagOrder($unfoundtags, $key, $root);
                        return false;
                    }
                    continue;
                } else {
                    $unfoundtags = $optionaltags = array();
                    $mismatch = false;
                }
                $key = next($keys);
                while ($key == 'attribs' || $key == '_contents') {
                    $key = next($keys);
                }
                if ($key && $key != $test['tag'] && isset($test['multiple'])) {
                    $unfoundtags[] = $test['tag'];
                    $optionaltags[] = $test['tag'];
                    $mismatch = true;
                }
                $ret &= $this->_processAttribs($test, $tag, $root);
                continue;
            }
        }
        if (!$mismatch && count($optionaltags)) {
            // don't error out on any optional tags
            $unfoundtags = array_diff($unfoundtags, $optionaltags);
        }
        if (count($unfoundtags)) {
            $this->_invalidTagOrder($unfoundtags, $key, $root);
        } elseif ($key) {
            // unknown tags
            $this->_invalidTagOrder('*no tags allowed here*', $key, $root);
            while ($key = next($keys)) {
                $this->_invalidTagOrder('*no tags allowed here*', $key, $root);
            }
        }
        return $ret;
    }

    function _processAttribs($choice, $tag, $context)
    {
        if (isset($choice['attribs'])) {
            if (!is_array($tag)) {
                $tag = array($tag);
            }
            $tags = $tag;
            if (!isset($tags[0])) {
                $tags = array($tags);
            }
            $ret = true;
            foreach ($tags as $i => $tag) {
                if (!is_array($tag) || !isset($tag['attribs'])) {
                    foreach ($choice['attribs'] as $attrib) {
                        if ($attrib{0} != '?') {
                            $ret &= $this->_tagHasNoAttribs($choice['tag'],
                                $context);
                            continue 2;
                        }
                    }
                }
                foreach ($choice['attribs'] as $attrib) {
                    if ($attrib{0} != '?') {
                        if (!isset($tag['attribs'][$attrib])) {
                            $ret &= $this->_tagMissingAttribute($choice['tag'],
                                $attrib, $context);
                        }
                    }
                }
            }
            return $ret;
        }
        return true;
    }

    function _processStructure($key)
    {
        $ret = array();
        if (count($pieces = explode('|', $key)) > 1) {
            foreach ($pieces as $piece) {
                $ret['choices'][] = $this->_processStructure($piece);
            }
            return $ret;
        }
        $multi = $key{0};
        if ($multi == '+' || $multi == '*') {
            $ret['multiple'] = $key{0};
            $key = substr($key, 1);
        }
        if (count($attrs = explode('->', $key)) > 1) {
            $ret['tag'] = array_shift($attrs);
            $ret['attribs'] = $attrs;
        } else {
            $ret['tag'] = $key;
        }
        return $ret;
    }

    function _validateStabilityVersion()
    {
        $structure = array('release', 'api');
        $a = $this->_stupidSchemaValidate($structure, $this->_packageInfo['version'], '<version>');
        $a &= $this->_stupidSchemaValidate($structure, $this->_packageInfo['stability'], '<stability>');
        if ($a) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $this->_packageInfo['version']['release'])) {
                $this->_invalidVersion('release', $this->_packageInfo['version']['release']);
            }
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $this->_packageInfo['version']['api'])) {
                $this->_invalidVersion('api', $this->_packageInfo['version']['api']);
            }
            if (!in_array($this->_packageInfo['stability']['release'],
                  array('snapshot', 'devel', 'alpha', 'beta', 'stable'))) {
                $this->_invalidState('release', $this->_packageinfo['stability']['release']);
            }
            if (!in_array($this->_packageInfo['stability']['api'],
                  array('devel', 'alpha', 'beta', 'stable'))) {
                $this->_invalidState('api', $this->_packageinfo['stability']['api']);
            }
        }
    }

    function _validateMaintainers()
    {
        $structure =
            array(
                'name',
                'user',
                'email',
                'active',
            );
        foreach (array('lead', 'developer', 'contributor', 'helper') as $type) {
            if (!isset($this->_packageInfo[$type])) {
                continue;
            }
            if (isset($this->_packageInfo[$type][0])) {
                foreach ($this->_packageInfo[$type] as $lead) {
                    $this->_stupidSchemaValidate($structure, $lead, '<' . $type . '>');
                }
            } else {
                $this->_stupidSchemaValidate($structure, $this->_packageInfo[$type],
                    '<' . $type . '>');
            }
        }
    }

    function _validatePhpDep($dep, $installcondition = false)
    {
        $structure = array(
            'min',
            '*max',
            '*exclude',
        );
        $type = $installcondition ? '<installcondition><php>' : '<dependencies><required><php>';
        $this->_stupidSchemaValidate($structure, $dep, $type);
        if (isset($dep['min'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?(?:-[a-zA-Z0-9]+)?$/',
                  $dep['min'])) {
                $this->_invalidVersion($type . '<min>', $dep['min']);
            }
        }
        if (isset($dep['max'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?(?:-[a-zA-Z0-9]+)?$/',
                  $dep['max'])) {
                $this->_invalidVersion($type . '<max>', $dep['max']);
            }
        }
    }

    function _validatePearinstallerDep($dep)
    {
        $structure = array(
            'min',
            '*max',
            '*recommended',
            '*exclude',
        );
        $this->_stupidSchemaValidate($structure, $dep, '<dependencies><required><pearinstaller>');
        if (isset($dep['min'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['min'])) {
                $this->_invalidVersion('<dependencies><required><pearinstaller><min>',
                    $dep['min']);
            }
        }
        if (isset($dep['max'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['max'])) {
                $this->_invalidVersion('<dependencies><required><pearinstaller><max>',
                    $dep['max']);
            }
        }
        if (isset($dep['recommended'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['recommended'])) {
                $this->_invalidVersion('<dependencies><required><pearinstaller><recommended>',
                    $dep['recommended']);
            }
        }
        if (isset($dep['exclude'])) {
            if (!is_array($dep['exclude'])) {
                $dep['exclude'] = array($dep['exclude']);
            }
            foreach ($dep['exclude'] as $exclude) {
                if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                      $exclude)) {
                    $this->_invalidVersion('<dependencies><required><pearinstaller><exclude>',
                        $exclude);
                }
            }
        }
    }

    function _validatePackageDep($dep, $group, $type = '<package>')
    {
        if (isset($dep['uri'])) {
            if (isset($dep['conflicts'])) {
                $structure = array(
                    'name',
                    'uri',
                    'conflicts',
                    '*providesextension',
                );
            } else {
                $structure = array(
                    'name',
                    'uri',
                    '*providesextension',
                );
            }
        } else {
            if (isset($dep['conflicts'])) {
                $structure = array(
                    'name',
                    'channel',
                    '*min',
                    '*max',
                    '*exclude',
                    'conflicts',
                    '*providesextension',
                );
            } else {
                $structure = array(
                    'name',
                    'channel',
                    '*min',
                    '*max',
                    '*recommended',
                    '*exclude',
                    '*nodefault',
                    '*providesextension',
                );
            }
        }
        if (isset($dep['name'])) {
            $type .= '<name>' . $dep['name'] . '</name>';
        }
        $this->_stupidSchemaValidate($structure, $dep, '<dependencies>' . $group . $type);
        if (isset($dep['uri']) && (isset($dep['min']) || isset($dep['max']) ||
              isset($dep['recommended']) || isset($dep['exclude']))) {
            $this->_uriDepsCannotHaveVersioning('<dependencies>' . $group . $type);
        }
        if (isset($dep['channel']) && strtolower($dep['channel']) == '__uri') {
            $this->_DepchannelCannotBeUri('<dependencies>' . $group . $type);
        }
        if (isset($dep['min'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['min'])) {
                $this->_invalidVersion('<dependencies>' . $group . $type . '<min>', $dep['min']);
            }
        }
        if (isset($dep['max'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['max'])) {
                $this->_invalidVersion('<dependencies>' . $group . $type . '<max>', $dep['max']);
            }
        }
        if (isset($dep['recommended'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['recommended'])) {
                $this->_invalidVersion('<dependencies>' . $group . $type . '<recommended>',
                    $dep['recommended']);
            }
        }
        if (isset($dep['exclude'])) {
            if (!is_array($dep['exclude'])) {
                $dep['exclude'] = array($dep['exclude']);
            }
            foreach ($dep['exclude'] as $exclude) {
                if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                      $exclude)) {
                    $this->_invalidVersion('<dependencies>' . $group . $type . '<exclude>',
                        $exclude);
                }
            }
        }
    }

    function _validateSubpackageDep($dep, $group)
    {
        $this->_validatePackageDep($dep, $group, '<subpackage>');
        if (isset($dep['providesextension'])) {
            $this->_subpackageCannotProvideExtension(@$dep['name']);
        }
        if (isset($dep['conflicts'])) {
            $this->_subpackagesCannotConflict(@$dep['name']);
        }
    }

    function _validateExtensionDep($dep, $group = false, $installcondition = false)
    {
        if (isset($dep['conflicts'])) {
            $structure = array(
                'name',
                '*min',
                '*max',
                '*exclude',
                'conflicts',
            );
        } else {
            $structure = array(
                'name',
                '*min',
                '*max',
                '*recommended',
                '*exclude',
            );
        }
        if ($installcondition) {
            $type = '<installcondition><extension>';
        } else {
            $type = '<dependencies>' . $group . '<extension>';
        }
        if (isset($dep['name'])) {
            $type .= '<name>' . $dep['name'] . '</name>';
        }
        $this->_stupidSchemaValidate($structure, $dep, $type);
        if (isset($dep['min'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['min'])) {
                $this->_invalidVersion(substr($type, 1) . '<min', $dep['min']);
            }
        }
        if (isset($dep['max'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['max'])) {
                $this->_invalidVersion(substr($type, 1) . '<max', $dep['max']);
            }
        }
        if (isset($dep['recommended'])) {
            if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                  $dep['recommended'])) {
                $this->_invalidVersion(substr($type, 1) . '<recommended', $dep['recommended']);
            }
        }
        if (isset($dep['exclude'])) {
            if (!is_array($dep['exclude'])) {
                $dep['exclude'] = array($dep['exclude']);
            }
            foreach ($dep['exclude'] as $exclude) {
                if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                      $exclude)) {
                    $this->_invalidVersion(substr($type, 1) . '<exclude', $exclude);
                }
            }
        }
    }

    function _validateOsDep($dep, $installcondition = false)
    {
        $structure = array(
            'name',
            '*conflicts',
        );
        $type = $installcondition ? '<installcondition><os>' : '<dependencies><required><os>';
        if ($this->_stupidSchemaValidate($structure, $dep, $type)) {
            if ($dep['name'] == '*') {
                if (array_key_exists('conflicts', $dep)) {
                    $this->_cannotConflictWithAllOs($type);
                }
            }
        }
    }

    function _validateArchDep($dep, $installcondition = false)
    {
        $structure = array(
            'pattern',
            '*conflicts',
        );
        $type = $installcondition ? '<installcondition><arch>' : '<dependencies><required><arch>';
        $this->_stupidSchemaValidate($structure, $dep, $type);
    }

    function _validateInstallConditions($cond, $release)
    {
        $structure = array(
            '*php',
            '*extension',
            '*os',
            '*arch',
        );
        if (!$this->_stupidSchemaValidate($structure,
              $cond, $release)) {
            return false;
        }
        foreach (array('php', 'extension', 'os', 'arch') as $type) {
            if (isset($cond[$type])) {
                $iter = $cond[$type];
                if (!is_array($iter) || !isset($iter[0])) {
                    $iter = array($iter);
                }
                foreach ($iter as $package) {
                    if ($type == 'extension') {
                        $this->{"_validate{$type}Dep"}($package, false, true);
                    } else {
                        $this->{"_validate{$type}Dep"}($package, true);
                    }
                }
            }
        }
    }

    function _validateDependencies()
    {
        $structure = array(
            'required',
            '*optional',
            '*group->name->hint'
        );
        if (!$this->_stupidSchemaValidate($structure,
              $this->_packageInfo['dependencies'], '<dependencies>')) {
            return false;
        }
        foreach (array('required', 'optional') as $simpledep) {
            if (isset($this->_packageInfo['dependencies'][$simpledep])) {
                if ($simpledep == 'optional') {
                    $structure = array(
                        '*package',
                        '*subpackage',
                        '*extension',
                    );
                } else {
                    $structure = array(
                        'php',
                        'pearinstaller',
                        '*package',
                        '*subpackage',
                        '*extension',
                        '*os',
                        '*arch',
                    );
                }
                if ($this->_stupidSchemaValidate($structure,
                      $this->_packageInfo['dependencies'][$simpledep],
                      "<dependencies><$simpledep>")) {
                    foreach (array('package', 'subpackage', 'extension') as $type) {
                        if (isset($this->_packageInfo['dependencies'][$simpledep][$type])) {
                            $iter = $this->_packageInfo['dependencies'][$simpledep][$type];
                            if (!isset($iter[0])) {
                                $iter = array($iter);
                            }
                            foreach ($iter as $package) {
                                if ($type != 'extension') {
                                    if (isset($package['uri'])) {
                                        if (isset($package['channel'])) {
                                            $this->_UrlOrChannel($type,
                                                $package['name']);
                                        }
                                    } else {
                                        if (!isset($package['channel'])) {
                                            $this->_NoChannel($type, $package['name']);
                                        }
                                    }
                                }
                                $this->{"_validate{$type}Dep"}($package, "<$simpledep>");
                            }
                        }
                    }
                    if ($simpledep == 'optional') {
                        continue;
                    }
                    foreach (array('php', 'pearinstaller', 'os', 'arch') as $type) {
                        if (isset($this->_packageInfo['dependencies'][$simpledep][$type])) {
                            $iter = $this->_packageInfo['dependencies'][$simpledep][$type];
                            if (!isset($iter[0])) {
                                $iter = array($iter);
                            }
                            foreach ($iter as $package) {
                                $this->{"_validate{$type}Dep"}($package);
                            }
                        }
                    }
                }
            }
        }
        if (isset($this->_packageInfo['dependencies']['group'])) {
            $groups = $this->_packageInfo['dependencies']['group'];
            if (!isset($groups[0])) {
                $groups = array($groups);
            }
            $structure = array(
                '*package',
                '*subpackage',
                '*extension',
            );
            foreach ($groups as $group) {
                if ($this->_stupidSchemaValidate($structure, $group, '<group>')) {
                    if (!PEAR_Validate::validGroupName($group['attribs']['name'])) {
                        $this->_invalidDepGroupName($group['attribs']['name']);
                    }
                    foreach (array('package', 'subpackage', 'extension') as $type) {
                        if (isset($group[$type])) {
                            $iter = $group[$type];
                            if (!isset($iter[0])) {
                                $iter = array($iter);
                            }
                            foreach ($iter as $package) {
                                if ($type != 'extension') {
                                    if (isset($package['uri'])) {
                                        if (isset($package['channel'])) {
                                            $this->_UrlOrChannelGroup($type,
                                                $package['name'],
                                                $group['name']);
                                        }
                                    } else {
                                        if (!isset($package['channel'])) {
                                            $this->_NoChannelGroup($type,
                                                $package['name'],
                                                $group['name']);
                                        }
                                    }
                                }
                                $this->{"_validate{$type}Dep"}($package, '<group name="' .
                                    $group['attribs']['name'] . '">');
                            }
                        }
                    }
                }
            }
        }
    }

    function _validateCompatible()
    {
        $compat = $this->_packageInfo['compatible'];
        if (!isset($compat[0])) {
            $compat = array($compat);
        }
        $required = array('name', 'channel', 'min', 'max', '*exclude');
        foreach ($compat as $package) {
            $type = '<compatible>';
            if (is_array($package) && array_key_exists('name', $package)) {
                $type .= '<name>' . $package['name'] . '</name>';
            }
            $this->_stupidSchemaValidate($required, $package, $type);
            if (is_array($package) && array_key_exists('min', $package)) {
                if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                      $package['min'])) {
                    $this->_invalidVersion(substr($type, 1) . '<min', $package['min']);
                }
            }
            if (is_array($package) && array_key_exists('max', $package)) {
                if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                      $package['max'])) {
                    $this->_invalidVersion(substr($type, 1) . '<max', $package['max']);
                }
            }
            if (is_array($package) && array_key_exists('exclude', $package)) {
                if (!is_array($package['exclude'])) {
                    $package['exclude'] = array($package['exclude']);
                }
                foreach ($package['exclude'] as $exclude) {
                    if (!preg_match('/^\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?$/',
                          $exclude)) {
                        $this->_invalidVersion(substr($type, 1) . '<exclude', $exclude);
                    }
                }
            }
        }
    }

    function _validateBundle($list)
    {
        if (!is_array($list) || !isset($list['bundledpackage'])) {
            return $this->_NoBundledPackages();
        }
        if (!is_array($list['bundledpackage']) || !isset($list['bundledpackage'][0])) {
            return $this->_AtLeast2BundledPackages();
        }
        foreach ($list['bundledpackage'] as $package) {
            if (!is_string($package)) {
                $this->_bundledPackagesMustBeFilename();
            }
        }
    }

    function _validateFilelist($list = false, $allowignore = false, $dirs = '')
    {
        $iscontents = false;
        if (!$list) {
            $iscontents = true;
            $list = $this->_packageInfo['contents'];
            if (isset($this->_packageInfo['bundle'])) {
                return $this->_validateBundle($list);
            }
        }
        if ($allowignore) {
            $struc = array(
                '*install->name->as',
                '*ignore->name'
            );
        } else {
            $struc = array(
                '*dir->name->?baseinstalldir',
                '*file->name->role->?baseinstalldir->?md5sum'
            );
            if (isset($list['dir']) && isset($list['file'])) {
                // stave off validation errors without requiring a set order.
                $_old = $list;
                if (isset($list['attribs'])) {
                    $list = array('attribs' => $_old['attribs']);
                }
                $list['dir'] = $_old['dir'];
                $list['file'] = $_old['file'];
            }
        }
        if (!isset($list['attribs']) || !isset($list['attribs']['name'])) {
            $unknown = $allowignore ? '<filelist>' : '<dir name="*unknown*">';
            $dirname = $iscontents ? '<contents>' : $unknown;
        } else {
            $dirname = '<dir name="' . $list['attribs']['name'] . '">';
        }
        $res = $this->_stupidSchemaValidate($struc, $list, $dirname);
        if ($allowignore && $res) {
            $this->_pf->getFilelist();
            $fcontents = $this->_pf->getContents();
            $filelist = array();
            if (!isset($fcontents['dir']['file'][0])) {
                $fcontents['dir']['file'] = array($fcontents['dir']['file']);
            }
            foreach ($fcontents['dir']['file'] as $file) {
                $filelist[$file['attribs']['name']] = true;
            }
            if (isset($list['install'])) {
                if (!isset($list['install'][0])) {
                    $list['install'] = array($list['install']);
                }
                foreach ($list['install'] as $file) {
                    if (!isset($filelist[$file['attribs']['name']])) {
                        $this->_notInContents($file['attribs']['name'], 'install');
                    }
                }
            }
            if (isset($list['ignore'])) {
                if (!isset($list['ignore'][0])) {
                    $list['ignore'] = array($list['ignore']);
                }
                foreach ($list['ignore'] as $file) {
                    if (!isset($filelist[$file['attribs']['name']])) {
                        $this->_notInContents($file['attribs']['name'], 'ignore');
                    }
                }
            }
        }
        if (!$allowignore && isset($list['file'])) {
            if (!isset($list['file'][0])) {
                // single file
                $list['file'] = array($list['file']);
            }
            foreach ($list['file'] as $i => $file)
            {
                if (isset($file['attribs']) && isset($file['attribs']['name']) &&
                      $file['attribs']['name']{0} == '.' &&
                        $file['attribs']['name']{1} == '/') {
                    // name is something like "./doc/whatever.txt"
                    $this->_invalidFileName($file['attribs']['name']);
                }
                if (isset($file['attribs']) && isset($file['attribs']['role'])) {
                    if (!$this->_validateRole($file['attribs']['role'])) {
                        if (isset($this->_packageInfo['usesrole'])) {
                            $roles = $this->_packageInfo['usesrole'];
                            if (!isset($roles[0])) {
                                $roles = array($roles);
                            }
                            foreach ($roles as $role) {
                                if ($role['role'] = $file['attribs']['role']) {
                                    $msg = 'This package contains role "%role%" and requires ' .
                                        'package "%package%" to be used';
                                    if (isset($role['uri'])) {
                                        $params = array('role' => $role['role'],
                                            'package' => $role['uri']);
                                    } else {
                                        $params = array('role' => $role['role'],
                                            'package' => $this->_pf->_registry->
                                            parsedPackageNameToString(array('package' =>
                                                $role['package'], 'channel' => $role['channel']),
                                                true));
                                    }
                                    $this->_stack->push('_mustInstallRole', 'error', $params, $msg);
                                }
                            }
                        }
                        $this->_invalidFileRole($file['attribs']['name'],
                            $dirname, $file['attribs']['role']);
                    }
                }
                if (!isset($file['attribs'])) {
                    continue;
                }
                $save = $file['attribs'];
                if ($dirs) {
                    $save['name'] = $dirs . '/' . $save['name'];
                }
                unset($file['attribs']);
                if (count($file) && $this->_curState != PEAR_VALIDATE_DOWNLOADING) { // has tasks
                    foreach ($file as $task => $value) {
                        if ($tagClass = $this->_pf->getTask($task)) {
                            if (!is_array($value) || !isset($value[0])) {
                                $value = array($value);
                            }
                            foreach ($value as $v) {
                                $ret = call_user_func(array($tagClass, 'validateXml'),
                                    $this->_pf, $v, $this->_pf->_config, $save);
                                if (is_array($ret)) {
                                    $this->_invalidTask($task, $ret, @$save['name']);
                                }
                            }
                        } else {
                            if (isset($this->_packageInfo['usestask'])) {
                                $roles = $this->_packageInfo['usestask'];
                                if (!isset($roles[0])) {
                                    $roles = array($roles);
                                }
                                foreach ($roles as $role) {
                                    if ($role['task'] = $task) {
                                        $msg = 'This package contains task "%task%" and requires ' .
                                            'package "%package%" to be used';
                                        if (isset($role['uri'])) {
                                            $params = array('task' => $role['task'],
                                                'package' => $role['uri']);
                                        } else {
                                            $params = array('task' => $role['task'],
                                                'package' => $this->_pf->_registry->
                                                parsedPackageNameToString(array('package' =>
                                                    $role['package'], 'channel' => $role['channel']),
                                                    true));
                                        }
                                        $this->_stack->push('_mustInstallTask', 'error',
                                            $params, $msg);
                                    }
                                }
                            }
                            $this->_unknownTask($task, $save['name']);
                        }
                    }
                }
            }
        }
        if (isset($list['ignore'])) {
            if (!$allowignore) {
                $this->_ignoreNotAllowed('ignore');
            }
        }
        if (isset($list['install'])) {
            if (!$allowignore) {
                $this->_ignoreNotAllowed('install');
            }
        }
        if (isset($list['file'])) {
            if ($allowignore) {
                $this->_fileNotAllowed('file');
            }
        }
        if (isset($list['dir'])) {
            if ($allowignore) {
                $this->_fileNotAllowed('dir');
            } else {
                if (!isset($list['dir'][0])) {
                    $list['dir'] = array($list['dir']);
                }
                foreach ($list['dir'] as $dir) {
                    if (isset($dir['attribs']) && isset($dir['attribs']['name'])) {
                        if ($dir['attribs']['name'] == '/' ||
                              !isset($this->_packageInfo['contents']['dir']['dir'])) {
                            // always use nothing if the filelist has already been flattened
                            $newdirs = '';
                        } elseif ($dirs == '') {
                            $newdirs = $dir['attribs']['name'];
                        } else {
                            $newdirs = $dirs . '/' . $dir['attribs']['name'];
                        }
                    } else {
                        $newdirs = $dirs;
                    }
                    $this->_validateFilelist($dir, $allowignore, $newdirs);
                }
            }
        }
    }

    function _validateRelease()
    {
        if (isset($this->_packageInfo['phprelease'])) {
            $release = 'phprelease';
            if (isset($this->_packageInfo['providesextension'])) {
                $this->_cannotProvideExtension($release);
            }
            if (isset($this->_packageInfo['srcpackage']) || isset($this->_packageInfo['srcuri'])) {
                $this->_cannotHaveSrcpackage($release);
            }
            $releases = $this->_packageInfo['phprelease'];
            if (!is_array($releases)) {
                return true;
            }
            if (!isset($releases[0])) {
                $releases = array($releases);
            }
            foreach ($releases as $rel) {
                $this->_stupidSchemaValidate(array(
                    '*installconditions',
                    '*filelist',
                ), $rel, '<phprelease>');
            }
        }
        if (isset($this->_packageInfo['extsrcrelease'])) {
            $release = 'extsrcrelease';
            if (!isset($this->_packageInfo['providesextension'])) {
                $this->_mustProvideExtension($release);
            }
            if (isset($this->_packageInfo['srcpackage']) || isset($this->_packageInfo['srcuri'])) {
                $this->_cannotHaveSrcpackage($release);
            }
            $releases = $this->_packageInfo['extsrcrelease'];
            if (!is_array($releases)) {
                return true;
            }
            if (!isset($releases[0])) {
                $releases = array($releases);
            }
            foreach ($releases as $rel) {
                $this->_stupidSchemaValidate(array(
                    '*installconditions',
                    '*configureoption->name->prompt->?default',
                    '*binarypackage',
                    '*filelist',
                ), $rel, '<extsrcrelease>');
                if (isset($rel['binarypackage'])) {
                    if (!is_array($rel['binarypackage']) || !isset($rel['binarypackage'][0])) {
                        $rel['binarypackage'] = array($rel['binarypackage']);
                    }
                    foreach ($rel['binarypackage'] as $bin) {
                        if (!is_string($bin)) {
                            $this->_binaryPackageMustBePackagename();
                        }
                    }
                }
            }
        }
        if (isset($this->_packageInfo['extbinrelease'])) {
            $release = 'extbinrelease';
            if (!isset($this->_packageInfo['providesextension'])) {
                $this->_mustProvideExtension($release);
            }
            if (isset($this->_packageInfo['channel']) &&
                  !isset($this->_packageInfo['srcpackage'])) {
                $this->_mustSrcPackage($release);
            }
            if (isset($this->_packageInfo['uri']) && !isset($this->_packageInfo['srcuri'])) {
                $this->_mustSrcuri($release);
            }
            $releases = $this->_packageInfo['extbinrelease'];
            if (!is_array($releases)) {
                return true;
            }
            if (!isset($releases[0])) {
                $releases = array($releases);
            }
            foreach ($releases as $rel) {
                $this->_stupidSchemaValidate(array(
                    '*installconditions',
                    '*filelist',
                ), $rel, '<extbinrelease>');
            }
        }
        if (isset($this->_packageInfo['bundle'])) {
            $release = 'bundle';
            if (isset($this->_packageInfo['providesextension'])) {
                $this->_cannotProvideExtension($release);
            }
            if (isset($this->_packageInfo['srcpackage']) || isset($this->_packageInfo['srcuri'])) {
                $this->_cannotHaveSrcpackage($release);
            }
            $releases = $this->_packageInfo['bundle'];
            if (!is_array($releases) || !isset($releases[0])) {
                $releases = array($releases);
            }
            foreach ($releases as $rel) {
                $this->_stupidSchemaValidate(array(
                    '*installconditions',
                    '*filelist',
                ), $rel, '<bundle>');
            }
        }
        foreach ($releases as $rel) {
            if (is_array($rel) && array_key_exists('installconditions', $rel)) {
                $this->_validateInstallConditions($rel['installconditions'],
                    "<$release><installconditions>");
            }
            if (is_array($rel) && array_key_exists('filelist', $rel)) {
                if ($rel['filelist']) {
                    
                    $this->_validateFilelist($rel['filelist'], true);
                }
            }
        }
    }

    /**
     * This is here to allow role extension through plugins
     * @param string
     */
    function _validateRole($role)
    {
        return in_array($role, PEAR_Installer_Role::getValidRoles($this->_pf->getPackageType()));
    }

    function _invalidTagOrder($oktags, $actual, $root)
    {
        $this->_stack->push(__FUNCTION__, 'error',
            array('oktags' => $oktags, 'actual' => $actual, 'root' => $root),
            'Invalid tag order in %root%, found <%actual%> expected one of "%oktags%"');
    }

    function _ignoreNotAllowed($type)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type),
            '<%type%> is not allowed inside global <contents>, only inside ' .
            '<phprelease>/<extbinrelease>, use <dir> and <file> only');
    }

    function _fileNotAllowed($type)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type),
            '<%type%> is not allowed inside release <filelist>, only inside ' .
            '<contents>, use <ignore> and <install> only');
    }

    function _tagMissingAttribute($tag, $attr, $context)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('tag' => $tag,
            'attribute' => $attr, 'context' => $context),
            'tag <%tag%> in context "%context%" has no attribute "%attribute%"');
    }

    function _tagHasNoAttribs($tag, $context)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('tag' => $tag,
            'context' => $context),
            'tag <%tag%> has no attributes in context "%context%"');
    }

    function _invalidInternalStructure()
    {
        $this->_stack->push(__FUNCTION__, 'exception', array(),
            'internal array was not generated by compatible parser, or extreme parser error, cannot continue');
    }

    function _invalidFileRole($file, $dir, $role)
    {
        $this->_stack->push(__FUNCTION__, 'error', array(
            'file' => $file, 'dir' => $dir, 'role' => $role,
            'roles' => PEAR_Installer_Role::getValidRoles($this->_pf->getPackageType())),
            'File "%file%" in directory "%dir%" has invalid role "%role%", should be one of %roles%');
    }

    function _invalidFileName($file, $dir)
    {
        $this->_stack->push(__FUNCTION__, 'error', array(
            'file' => $file),
            'File "%file%" cannot begin with "."');
    }

    function _filelistCannotContainFile($filelist)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('tag' => $filelist),
            '<%tag%> can only contain <dir>, contains <file>.  Use ' .
            '<dir name="/"> as the first dir element');
    }

    function _filelistMustContainDir($filelist)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('tag' => $filelist),
            '<%tag%> must contain <dir>.  Use <dir name="/"> as the ' .
            'first dir element');
    }

    function _tagCannotBeEmpty($tag)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('tag' => $tag),
            '<%tag%> cannot be empty (<%tag%/>)');
    }

    function _UrlOrChannel($type, $name)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type,
            'name' => $name),
            'Required dependency <%type%> "%name%" can have either url OR ' .
            'channel attributes, and not both');
    }

    function _NoChannel($type, $name)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type,
            'name' => $name),
            'Required dependency <%type%> "%name%" must have either url OR ' .
            'channel attributes');
    }

    function _UrlOrChannelGroup($type, $name, $group)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type,
            'name' => $name, 'group' => $group),
            'Group "%group%" dependency <%type%> "%name%" can have either url OR ' .
            'channel attributes, and not both');
    }

    function _NoChannelGroup($type, $name, $group)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type,
            'name' => $name, 'group' => $group),
            'Group "%group%" dependency <%type%> "%name%" must have either url OR ' .
            'channel attributes');
    }

    function _unknownChannel($channel)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('channel' => $channel),
            'Unknown channel "%channel%"');
    }

    function _noPackageVersion()
    {
        $this->_stack->push(__FUNCTION__, 'error', array(),
            'package.xml <package> tag has no version attribute, or version is not 2.0');
    }

    function _NoBundledPackages()
    {
        $this->_stack->push(__FUNCTION__, 'error', array(),
            'No <bundledpackage> tag was found in <contents>, required for bundle packages');
    }

    function _AtLeast2BundledPackages()
    {
        $this->_stack->push(__FUNCTION__, 'error', array(),
            'At least 2 packages must be bundled in a bundle package');
    }

    function _ChannelOrUri($name)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('name' => $name),
            'Bundled package "%name%" can have either a uri or a channel, not both');
    }

    function _noChildTag($child, $tag)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('child' => $child, 'tag' => $tag),
            'Tag <%tag%> is missing child tag <%child%>');
    }

    function _invalidVersion($type, $value)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type, 'value' => $value),
            'Version type <%type%> is not a valid version (%value%)');
    }

    function _invalidState($type, $value)
    {
        $states = array('stable', 'beta', 'alpha', 'devel');
        if ($type != 'api') {
            $states[] = 'snapshot';
        }
        if (strtolower($value) == 'rc') {
            $this->_stack->push(__FUNCTION__, 'error',
                array('version' => $this->_packageInfo['version']['release']),
                'RC is not a state, it is a version postfix, try %version%RC1, stability beta');
        }
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type, 'value' => $value,
            'types' => $states),
            'Stability type <%type%> is not a valid stability (%value%), must be one of ' .
            '%types%');
    }

    function _invalidTask($task, $ret, $file)
    {
        switch ($ret[0]) {
            case PEAR_TASK_ERROR_MISSING_ATTRIB :
                $info = array('attrib' => $ret[1], 'task' => $task, 'file' => $file);
                $msg = 'task <%task%> is missing attribute "%attrib%" in file %file%';
            break;
            case PEAR_TASK_ERROR_NOATTRIBS :
                $info = array('task' => $task, 'file' => $file);
                $msg = 'task <%task%> has no attributes in file %file%';
            break;
            case PEAR_TASK_ERROR_WRONG_ATTRIB_VALUE :
                $info = array('attrib' => $ret[1], 'values' => $ret[3],
                    'was' => $ret[2], 'task' => $task, 'file' => $file);
                $msg = 'task <%task%> attribute "%attrib%" has the wrong value "%was%" '.
                    'in file %file%, expecting one of "%values%"';
            break;
            case PEAR_TASK_ERROR_INVALID :
                $info = array('reason' => $ret[1], 'task' => $task, 'file' => $file);
                $msg = 'task <%task%> in file %file% is invalid because of "%reason%"';
            break;
        }
        $this->_stack->push(__FUNCTION__, 'error', $info, $msg);
    }

    function _unknownTask($task, $file)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('task' => $task, 'file' => $file),
            'Unknown task "%task%" passed in file <file name="%file%">');
    }

    function _subpackageCannotProvideExtension($name)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('name' => $name),
            'Subpackage dependency "%name%" cannot use <providesextension>, ' .
            'only package dependencies can use this tag');
    }

    function _subpackagesCannotConflict($name)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('name' => $name),
            'Subpackage dependency "%name%" cannot use <conflicts/>, ' .
            'only package dependencies can use this tag');
    }

    function _cannotProvideExtension($release)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('release' => $release),
            '<%release%> packages cannot use <providesextension>, only extbinrelease and extsrcrelease can provide a PHP extension');
    }

    function _mustProvideExtension($release)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('release' => $release),
            '<%release%> packages must use <providesextension> to indicate which PHP extension is provided');
    }

    function _cannotHaveSrcpackage($release)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('release' => $release),
            '<%release%> packages cannot specify a source code package, only extension binaries may use the <srcpackage> tag');
    }

    function _mustSrcPackage($release)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('release' => $release),
            '<extbinrelease> packages must specify a source code package with <srcpackage>');
    }

    function _mustSrcuri($release)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('release' => $release),
            '<extbinrelease> packages must specify a source code package with <srcuri>');
    }

    function _uriDepsCannotHaveVersioning($type)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type),
            '%type%: dependencies with a <uri> tag cannot have any versioning information');
    }

    function _conflictingDepsCannotHaveVersioning($type)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type),
            '%type%: conflicting dependencies cannot have versioning info, use <exclude> to ' .
            'exclude specific versions of a dependency');
    }

    function _DepchannelCannotBeUri($type)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('type' => $type),
            '%type%: channel cannot be __uri, this is a pseudo-channel reserved for uri ' .
            'dependencies only');
    }

    function _bundledPackagesMustBeFilename()
    {
        $this->_stack->push(__FUNCTION__, 'error', array(),
            '<bundledpackage> tags must contain only the filename of a package release ' .
            'in the bundle');
    }

    function _binaryPackageMustBePackagename()
    {
        $this->_stack->push(__FUNCTION__, 'error', array(),
            '<binarypackage> tags must contain the name of a package that is ' .
            'a compiled version of this extsrc package');
    }

    function _fileNotFound($file)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('file' => $file),
            'File "%file%" in package.xml does not exist');
    }

    function _notInContents($file, $tag)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('file' => $file, 'tag' => $tag),
            '<%tag% name="%file%"> is invalid, file is not in <contents>');
    }

    function _cannotValidateNoPathSet()
    {
        $this->_stack->push(__FUNCTION__, 'error', array(),
            'Cannot validate files, no path to package file is set (use setPackageFile())');
    }

    function _usesroletaskMustHaveChannelOrUri($role, $tag)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('role' => $role, 'tag' => $tag),
            '<%tag%> must contain either <uri>, or <channel> and <package>');
    }

    function _usesroletaskMustHavePackage($role, $tag)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('role' => $role, 'tag' => $tag),
            '<%tag%> must contain <package>');
    }

    function _usesroletaskMustHaveRoleTask($tag, $type)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('tag' => $tag, 'type' => $type),
            '<%tag%> must contain <%type%> defining the %type% to be used');
    }

    function _cannotConflictWithAllOs($type)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('tag' => $tag),
            '%tag% cannot conflict with all OSes');
    }

    function _invalidDepGroupName($name)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('group' => $name),
            'Invalid dependency group name "%name%"');
    }

    function _analyzeBundledPackages()
    {
        if (!$this->_isValid) {
            return false;
        }
        if (!$this->_pf->getPackageType() == 'bundle') {
            return false;
        }
        if (!isset($this->_pf->_packageFile)) {
            return false;
        }
        $dir_prefix = dirname($this->_pf->_packageFile);
        $log = isset($this->_pf->_logger) ? array(&$this->_pf->_logger, 'log') :
            array('PEAR_Common', 'log');
        $info = $this->_pf->getContents();
        $info = $info['bundledpackage'];
        if (!is_array($info)) {
            $info = array($info);
        }
        $pkg = &new PEAR_PackageFile($this->_pf->_config);
        foreach ($info as $package) {
            if (!file_exists($dir_prefix . DIRECTORY_SEPARATOR . $package)) {
                $this->_fileNotFound($dir_prefix . DIRECTORY_SEPARATOR . $package);
                $this->_isValid = 0;
                continue;
            }
            call_user_func_array($log, array(1, "Analyzing bundled package $package"));
            PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
            $ret = $pkg->fromAnyFile($dir_prefix . DIRECTORY_SEPARATOR . $package,
                PEAR_VALIDATE_NORMAL);
            PEAR::popErrorHandling();
            if (PEAR::isError($ret)) {
                call_user_func_array($log, array(0, "ERROR: package $package is not a valid " .
                    'package'));
                $inf = $ret->getUserInfo();
                if (is_array($inf)) {
                    foreach ($inf as $err) {
                        call_user_func_array($log, array(1, $err['message']));
                    }
                }
                return false;
            }
        }
        return true;
    }

    function _analyzePhpFiles()
    {
        if (!$this->_isValid) {
            return false;
        }
        if (!isset($this->_pf->_packageFile)) {
            $this->_cannotValidateNoPathSet();
            return false;
        }
        $dir_prefix = dirname($this->_pf->_packageFile);
        $common = new PEAR_Common;
        $log = isset($this->_pf->_logger) ? array(&$this->_pf->_logger, 'log') :
            array(&$common, 'log');
        $info = $this->_pf->getContents();
        $info = $info['dir']['file'];
        if (isset($info['attribs'])) {
            $info = array($info);
        }
        $provides = array();
        foreach ($info as $fa) {
            $fa = $fa['attribs'];
            $file = $fa['name'];
            if (!file_exists($dir_prefix . DIRECTORY_SEPARATOR . $file)) {
                $this->_fileNotFound($dir_prefix . DIRECTORY_SEPARATOR . $file);
                $this->_isValid = 0;
                continue;
            }
            if (in_array($fa['role'], PEAR_Installer_Role::getPhpRoles()) && $dir_prefix) {
                call_user_func_array($log, array(1, "Analyzing $file"));
                $srcinfo = $this->analyzeSourceCode($dir_prefix . DIRECTORY_SEPARATOR . $file);
                if ($srcinfo) {
                    $provides = array_merge($provides, $this->_buildProvidesArray($srcinfo));
                }
            }
        }
        $this->_packageName = $pn = $this->_pf->getPackage();
        $pnl = strlen($pn);
        foreach ($provides as $key => $what) {
            if (isset($what['explicit']) || !$what) {
                // skip conformance checks if the provides entry is
                // specified in the package.xml file
                continue;
            }
            extract($what);
            if ($type == 'class') {
                if (!strncasecmp($name, $pn, $pnl)) {
                    continue;
                }
                $this->_stack->push(__FUNCTION__, 'warning',
                    array('file' => $file, 'type' => $type, 'name' => $name, 'package' => $pn),
                    'in %file%: %type% "%name%" not prefixed with package name "%package%"');
            } elseif ($type == 'function') {
                if (strstr($name, '::') || !strncasecmp($name, $pn, $pnl)) {
                    continue;
                }
                $this->_stack->push(__FUNCTION__, 'warning',
                    array('file' => $file, 'type' => $type, 'name' => $name, 'package' => $pn),
                    'in %file%: %type% "%name%" not prefixed with package name "%package%"');
            }
        }
        return $this->_isValid;
    }

    /**
     * Analyze the source code of the given PHP file
     *
     * @param  string Filename of the PHP file
     * @param  boolean whether to analyze $file as the file contents
     * @return mixed
     */
    function analyzeSourceCode($file, $string = false)
    {
        if (!function_exists("token_get_all")) {
            return false;
        }
        if (!defined('T_DOC_COMMENT')) {
            define('T_DOC_COMMENT', T_COMMENT);
        }
        if (!defined('T_INTERFACE')) {
            define('T_INTERFACE', -1);
        }
        if (!defined('T_IMPLEMENTS')) {
            define('T_IMPLEMENTS', -1);
        }
        if ($string) {
            $contents = $file;
        } else {
            if (!$fp = @fopen($file, "r")) {
                return false;
            }
            if (function_exists('file_get_contents')) {
                fclose($fp);
                $contents = file_get_contents($file);
            } else {
                $contents = @fread($fp, filesize($file));
                fclose($fp);
            }
        }
        $tokens = token_get_all($contents);
/*
        for ($i = 0; $i < sizeof($tokens); $i++) {
            @list($token, $data) = $tokens[$i];
            if (is_string($token)) {
                var_dump($token);
            } else {
                print token_name($token) . ' ';
                var_dump(rtrim($data));
            }
        }
*/
        $look_for = 0;
        $paren_level = 0;
        $bracket_level = 0;
        $brace_level = 0;
        $lastphpdoc = '';
        $current_class = '';
        $current_interface = '';
        $current_class_level = -1;
        $current_function = '';
        $current_function_level = -1;
        $declared_classes = array();
        $declared_interfaces = array();
        $declared_functions = array();
        $declared_methods = array();
        $used_classes = array();
        $used_functions = array();
        $extends = array();
        $implements = array();
        $nodeps = array();
        $inquote = false;
        $interface = false;
        for ($i = 0; $i < sizeof($tokens); $i++) {
            if (is_array($tokens[$i])) {
                list($token, $data) = $tokens[$i];
            } else {
                $token = $tokens[$i];
                $data = '';
            }
            if ($inquote) {
                if ($token != '"' && $token != T_END_HEREDOC) {
                    continue;
                } else {
                    $inquote = false;
                    continue;
                }
            }
            switch ($token) {
                case T_WHITESPACE :
                    continue;
                case ';':
                    if ($interface) {
                        $current_function = '';
                        $current_function_level = -1;
                    }
                    break;
                case '"':
                case T_START_HEREDOC:
                    $inquote = true;
                    break;
                case T_CURLY_OPEN:
                case T_DOLLAR_OPEN_CURLY_BRACES:
                case '{': $brace_level++; continue 2;
                case '}':
                    $brace_level--;
                    if ($current_class_level == $brace_level) {
                        $current_class = '';
                        $current_class_level = -1;
                    }
                    if ($current_function_level == $brace_level) {
                        $current_function = '';
                        $current_function_level = -1;
                    }
                    continue 2;
                case '[': $bracket_level++; continue 2;
                case ']': $bracket_level--; continue 2;
                case '(': $paren_level++;   continue 2;
                case ')': $paren_level--;   continue 2;
                case T_INTERFACE:
                    $interface = true;
                case T_CLASS:
                    if (($current_class_level != -1) || ($current_function_level != -1)) {
                        $this->_stack->push(__FUNCTION__, 'error', array('file' => $file),
                        'Parser error: invalid PHP found in file "%file%"');
                        return false;
                    }
                case T_FUNCTION:
                case T_NEW:
                case T_EXTENDS:
                case T_IMPLEMENTS:
                    $look_for = $token;
                    continue 2;
                case T_STRING:
                    if (version_compare(zend_version(), '2.0', '<')) {
                        if (in_array(strtolower($data),
                            array('public', 'private', 'protected', 'abstract',
                                  'interface', 'implements', 'throw') 
                                 )) {
                            $this->_stack->push(__FUNCTION__, 'warning', array(
                                'file' => $file),
                                'Error, PHP5 token encountered in %file%,' .
                                ' analysis should be in PHP5');
                        }
                    }
                    if ($look_for == T_CLASS) {
                        $current_class = $data;
                        $current_class_level = $brace_level;
                        $declared_classes[] = $current_class;
                    } elseif ($look_for == T_INTERFACE) {
                        $current_interface = $data;
                        $current_class_level = $brace_level;
                        $declared_interfaces[] = $current_interface;
                    } elseif ($look_for == T_IMPLEMENTS) {
                        $implements[$current_class] = $data;
                    } elseif ($look_for == T_EXTENDS) {
                        $extends[$current_class] = $data;
                    } elseif ($look_for == T_FUNCTION) {
                        if ($current_class) {
                            $current_function = "$current_class::$data";
                            $declared_methods[$current_class][] = $data;
                        } elseif ($current_interface) {
                            $current_function = "$current_interface::$data";
                            $declared_methods[$current_interface][] = $data;
                        } else {
                            $current_function = $data;
                            $declared_functions[] = $current_function;
                        }
                        $current_function_level = $brace_level;
                        $m = array();
                    } elseif ($look_for == T_NEW) {
                        $used_classes[$data] = true;
                    }
                    $look_for = 0;
                    continue 2;
                case T_VARIABLE:
                    $look_for = 0;
                    continue 2;
                case T_DOC_COMMENT:
                case T_COMMENT:
                    if (preg_match('!^/\*\*\s!', $data)) {
                        $lastphpdoc = $data;
                        if (preg_match_all('/@nodep\s+(\S+)/', $lastphpdoc, $m)) {
                            $nodeps = array_merge($nodeps, $m[1]);
                        }
                    }
                    continue 2;
                case T_DOUBLE_COLON:
                    if (!($tokens[$i - 1][0] == T_WHITESPACE || $tokens[$i - 1][0] == T_STRING)) {
                        $this->_stack->push(__FUNCTION__, 'warning', array('file' => $file),
                            'Parser error: invalid PHP found in file "%file%"');
                        return false;
                    }
                    $class = $tokens[$i - 1][1];
                    if (strtolower($class) != 'parent') {
                        $used_classes[$class] = true;
                    }
                    continue 2;
            }
        }
        return array(
            "source_file" => $file,
            "declared_classes" => $declared_classes,
            "declared_interfaces" => $declared_interfaces,
            "declared_methods" => $declared_methods,
            "declared_functions" => $declared_functions,
            "used_classes" => array_diff(array_keys($used_classes), $nodeps),
            "inheritance" => $extends,
            "implements" => $implements,
            );
    }

    /**
     * Build a "provides" array from data returned by
     * analyzeSourceCode().  The format of the built array is like
     * this:
     *
     *  array(
     *    'class;MyClass' => 'array('type' => 'class', 'name' => 'MyClass'),
     *    ...
     *  )
     *
     *
     * @param array $srcinfo array with information about a source file
     * as returned by the analyzeSourceCode() method.
     *
     * @return void
     *
     * @access private
     *
     */
    function _buildProvidesArray($srcinfo)
    {
        if (!$this->_isValid) {
            return array();
        }
        $providesret = array();
        $file = basename($srcinfo['source_file']);
        $pn = $this->_pf->getPackage();
        $pnl = strlen($pn);
        foreach ($srcinfo['declared_classes'] as $class) {
            $key = "class;$class";
            if (isset($providesret[$key])) {
                continue;
            }
            $providesret[$key] =
                array('file'=> $file, 'type' => 'class', 'name' => $class);
            if (isset($srcinfo['inheritance'][$class])) {
                $providesret[$key]['extends'] =
                    $srcinfo['inheritance'][$class];
            }
        }
        foreach ($srcinfo['declared_methods'] as $class => $methods) {
            foreach ($methods as $method) {
                $function = "$class::$method";
                $key = "function;$function";
                if ($method{0} == '_' || !strcasecmp($method, $class) ||
                    isset($providesret[$key])) {
                    continue;
                }
                $providesret[$key] =
                    array('file'=> $file, 'type' => 'function', 'name' => $function);
            }
        }

        foreach ($srcinfo['declared_functions'] as $function) {
            $key = "function;$function";
            if ($function{0} == '_' || isset($providesret[$key])) {
                continue;
            }
            if (!strstr($function, '::') && strncasecmp($function, $pn, $pnl)) {
                $warnings[] = "in1 " . $file . ": function \"$function\" not prefixed with package name \"$pn\"";
            }
            $providesret[$key] =
                array('file'=> $file, 'type' => 'function', 'name' => $function);
        }
        return $providesret;
    }
}
?>