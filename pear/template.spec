Summary: PEAR: @summary@
Name: @rpm_package@
Version: @version@
Release: 1
License: @release_license@
Group: Development/Libraries
Source: http://@master_server@/get/@package@-%{version}.tgz
BuildRoot: %{_tmppath}/%{name}-root
URL: http://@master_server@/
Prefix: %{_prefix}
Docdir: @doc_dir@/@package@
BuildArchitectures: @arch@
@extra_headers@

%description
@description@

%prep
#rm -rf Console_Getopt-%{version} package.xml
#mkdir -p Console_Getopt-%{version}
#ln -s Console_Getopt-%{version}/package.xml package.xml
%setup -q -D -n @package@-%{version}
mv ../package.xml .

%build
echo BuildRoot=%{buildroot}

%post
pear uninstall --nodeps -r @package@

%postun
pear install --nodeps -r @rpm_xml_dir@/@package@.xml

%install
rm -rf %{buildroot}/*
pear -q install -R %{buildroot} -n package.xml
mkdir -p %{buildroot}@rpm_xml_dir@
cp -p package.xml %{buildroot}@rpm_xml_dir@/@package@.xml

%files
%defattr(-,root,root)
%doc @doc_files@
@files@
@rpm_xml_dir@/@package@.xml
