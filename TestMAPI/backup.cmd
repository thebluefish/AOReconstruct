backsrc TestMAPI
move /Y TestMAPI.zip MAPIEx\Release
cd MAPIEx\Release
zip -X MAPIExDLL.zip MAPIEx.dll MAPIEx.lib
if %1.==vcr. copy /Y *.zip \\vcr\z$\Backup
pause
