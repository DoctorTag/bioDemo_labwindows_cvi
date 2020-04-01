<distribution version="17.0.0" name="Bio_cvi" type="MSI64">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{F53BBC23-11D4-431F-A801-36B391D25368}" x64="true">
		<general appName="BioSensor_cvi" outputLocation="d:\software\2020\cvi\Demo_labwindows_cvi\cvidistkit.BioSensor_cvi" relOutputLocation="cvidistkit.BioSensor_cvi" outputLocationWithVars="d:\software\2020\cvi\Demo_labwindows_cvi\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.2">
			<arp company="Microsoft" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="true" showRuntimeOnly="true" readMe="" license="">
			<dlgstrings welcomeTitle="BioSensor_cvi" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="Bio_cvi" dirID="100" parentID="24" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="Bio_cvi" dirID="101" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="24" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="d:\software\2020\cvi\Demo_labwindows_cvi\cvibuild.bio_cvi\Release64\bio_demo_64bits.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="d:\software\2020\cvi\Demo_labwindows_cvi\bio_demo.uir" relSourcePath="bio_demo.uir" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput64 targetType="0" dirID="100" projectID="0">
				<fileID>0</fileID></projectOutput64>
			<projectDependencies64 dirID="100" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="Bio_cvi" targetFileID="0" destDirID="101" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine installToAppDir="false" activeXsup="false" analysis="true" cvirte="true" dotnetsup="true" instrsup="true" lowlevelsup="true" lvrt="true" netvarsup="true" rtutilsup="false">
			<hasSoftDeps/>
			<doNotAutoSelect>
			<component>activeXsup</component>
			<component>rtutilsup</component>
			</doNotAutoSelect></runtimeEngine><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WIN7_SP1</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<baselineProducts>
			<product name="NI LabWindows/CVI Shared Runtime 2017" UC="{80D3D303-75B9-4607-9312-E5FC68E5BFD2}" productID="{28BFBCB6-5EEE-47F1-A894-FCD877057827}" path="(None)" flavorID="_full_" flavorName="Full" verRestr="false" coreVer="17.0.49157">
				<dependencies>
					<productID>{13774AE4-124D-48C2-A8AC-F8070918385E}</productID>
					<productID>{18C5DCAB-EF32-48F8-9789-2A33DE1CA2CB}</productID>
					<productID>{29FBD1D9-6A19-4CF6-935F-C0EFCFB6CA3D}</productID>
					<productID>{2D894BCB-6069-418C-BF94-8E5915D8867C}</productID>
					<productID>{2FC99AB8-F11D-4938-B14F-AC60D1016FE7}</productID>
					<productID>{35E63DC2-6BCC-48D6-A1C5-5B068DF34117}</productID>
					<productID>{4500C77E-71CE-41A9-AD2A-B7D9C2E3397E}</productID>
					<productID>{4A548403-7448-4B6D-8A8E-F41F53AE4A3F}</productID>
					<productID>{5E99102B-D07A-4D85-8D50-4BE55E3633A0}</productID>
					<productID>{5FE7B722-6061-4AFA-A26F-2979CB4FEA40}</productID>
					<productID>{68428815-FD93-4897-BC4A-EC5E3194C4E7}</productID>
					<productID>{6EFCD9C0-82FC-4C51-B7F4-3377B6D03BA6}</productID>
					<productID>{92F3B1E4-20BE-44F2-8D2F-F0335FE172A2}</productID>
					<productID>{A4FAF873-938B-43B8-BBF2-E4B2C0CE730B}</productID>
					<productID>{BDADF4B2-9B31-48B3-8E3E-D15047C4C8AD}</productID>
					<productID>{BEA1952D-1F00-4336-9A76-C41FA181C02D}</productID>
					<productID>{CE7D38A4-66ED-4D33-A385-D4A2CF4BFF81}</productID>
					<productID>{D357A74D-E186-4E4B-AF78-E035D3E83368}</productID>
					<productID>{D4E86102-76C1-458C-B0C2-F42B43CAD3CC}</productID>
					<productID>{DA09F5A1-AA07-467D-A0B5-31D9EC8B5048}</productID>
					<productID>{DBAAC357-96E7-459F-B115-B14E29F652EF}</productID>
					<productID>{E2821E0B-CB08-41BD-AE01-A9068F7B8771}</productID>
					<productID>{F05EC50F-B2B4-4B87-A32C-6F9AD11D1B49}</productID>
					<productID>{FE7D71A2-BF64-441C-9489-BE6F6937B98E}</productID>
					<productID>{FF35E2A2-9214-4B76-91AE-5567076AAE8C}</productID>
					<productID>{FF56A0C1-3413-4024-89A6-0FB565B2E78C}</productID>
					<productID>{FF8EF684-CF13-4EE7-BC1D-26AB49EB0CD4}</productID></dependencies></product></baselineProducts>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="d:\software\2020\cvi\Demo_labwindows_cvi\bio_cvi.prj" ProjectRelativePath="bio_cvi.prj"/></Projects>
		<buildData progressBarRate="0.725152952251945">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.054833500000000</ProductsAdded>
				<DPConfigured>3.129824750000000</DPConfigured>
				<DPMergeModulesAdded>4.649827749999999</DPMergeModulesAdded>
				<DPClosed>10.612595249999996</DPClosed>
				<DistributionsCopied>13.038487750000000</DistributionsCopied>
				<End>137.901941499999992</End></progressTimes></buildData>
	</msi>
</distribution>
