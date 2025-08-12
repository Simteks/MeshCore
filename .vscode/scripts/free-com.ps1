param(
  [string]$ComPort = "COM8"
)
$ErrorActionPreference = 'SilentlyContinue'
# Processes that often hold serial ports or PIO monitor
$targets = @(
  'putty.exe','tio.exe','screen.exe','teraterm.exe','mobaxterm.exe','miniterm.py',
  'picocom.exe','platformio.exe','pio.exe','esptool.py','python.exe'
)

# Find processes by command line mention of COM port or by common target name
$procs = Get-CimInstance Win32_Process | Where-Object {
  ($_.CommandLine -match [regex]::Escape($ComPort)) -or
  ($_.CommandLine -match 'device\s+monitor') -or
  ($targets -contains ($_.Name.ToLower()))
}

foreach ($p in $procs) {
  # Avoid killing our own shell or VS Code host
  if ($p.Name -match 'Code' -or $p.Name -match 'powershell' -or $p.ProcessId -eq $PID) { continue }
  try { Stop-Process -Id $p.ProcessId -Force -ErrorAction SilentlyContinue } catch {}
}
Start-Sleep -Milliseconds 200
