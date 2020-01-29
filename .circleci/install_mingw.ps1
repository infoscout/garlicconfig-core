if (Test-Path C:\mingw\bin) { "MinGW Already Exists." }
else {
  $client = New-Object System.Net.WebClient
  $client.DownloadFile("https://osdn.net/frs/redir.php?m=constant&f=mingw%2F68260%2Fmingw-get-0.6.3-mingw32-pre-20170905-1-bin.zip", "C:\mingw.zip")
  mkdir C:\mingw
  unzip C:\mingw.zip -d C:\mingw
  cd C:\mingw\bin\
  .\mingw-get.exe install gcc g++ mingw32-make
}
