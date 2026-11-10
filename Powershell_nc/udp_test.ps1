# Petit client UDP de test (remplace netcat sous Windows, sans droits admin)
# Lancement : powershell -ExecutionPolicy Bypass -File udp_test.ps1 <IP> <port>
# Exemple   : powershell -ExecutionPolicy Bypass -File udp_test.ps1 192.168.1.100 8000
param([string]$ip = "192.168.1.100", [int]$port = 8000)

$u = New-Object System.Net.Sockets.UdpClient
$u.Client.ReceiveTimeout = 2000            # 2 s d'attente max
$ep = New-Object System.Net.IPEndPoint([Net.IPAddress]::Any, 0)
Write-Host "Cible $ip`:$port - tapez une commande (ANGLE 45, DIST...), vide pour quitter"

while ($true) {
    $cmd = Read-Host ">"
    if ($cmd -eq "") { break }
    $b = [Text.Encoding]::ASCII.GetBytes("$cmd`n")
    $u.Send($b, $b.Length, $ip, $port) | Out-Null
    try {
        $rep = $u.Receive([ref]$ep)
        Write-Host ([Text.Encoding]::ASCII.GetString($rep).TrimEnd()) "(de $ep)"
    } catch {
        Write-Host "Pas de reponse (timeout 2 s)"
    }
}
$u.Close()
