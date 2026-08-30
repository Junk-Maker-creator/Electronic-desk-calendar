$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$sketchPath = Join-Path $repoRoot 'WuhanDeskPanel'
$librariesPath = Join-Path $repoRoot '_Complete\Arduino_libraries'
$outputPath = Join-Path $repoRoot 'build'

if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
    throw 'arduino-cli is not on PATH. Install Arduino CLI first, then run this script again.'
}

if (-not (Test-Path -LiteralPath (Join-Path $sketchPath 'WuhanDeskPanel.ino'))) {
    throw "Sketch not found: $sketchPath"
}

if (-not (Test-Path -LiteralPath $librariesPath)) {
    throw "Bundled libraries not found: $librariesPath"
}

arduino-cli compile `
    --clean `
    --fqbn 'esp32:esp32:waveshare_esp32_s3_touch_lcd_7' `
    --board-options 'PSRAM=enabled,FlashMode=qio,FlashSize=8M,PartitionScheme=huge_app' `
    --libraries $librariesPath `
    --output-dir $outputPath `
    $sketchPath

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Output "Build complete: $outputPath"
