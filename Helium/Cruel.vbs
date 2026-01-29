Set objOutlook = CreateObject("Outlook.Application")
Set objMail = objOutlook.CreateItem(0)

' Configurações do e-mail
objMail.To = "osjanelas@gmail.com"
objMail.Subject = "Helium"
objMail.Body = "HELP"

objMail.Display

MsgBox "HELP", 64, "HELP"