#!/usr/bin/env python3

import cgi
import cgitb

# Activer les traces en cas d'erreur pour le débogage
cgitb.enable()

# En-tête HTTP indiquant que la réponse est du texte HTML
print("Content-Type: text/html")
print()

# Contenu HTML de la page
print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("<title>Exécution de script Python via CGI</title>")
print("</head>")
print("<body>")
print("<h1>Exécution de script Python via CGI</h1>")

# Obtenir les données du formulaire
form = cgi.FieldStorage()

# Vérifier si un script a été soumis
if "script" in form:
    # Récupérer le contenu du script
    script_content = form["script"].value

    try:
        # Exécuter le script Python
        exec(script_content)
        print("<p>Script exécuté avec succès !</p>")
    except Exception as e:
        print(f"<p>Erreur lors de l'exécution du script : {e}</p>")
else:
    # Afficher le formulaire pour soumettre un script
    print("<form method='post'>")
    print("<textarea name='script' rows='10' cols='50'></textarea><br>")
    print("<input type='submit' value='Exécuter le script'>")
    print("</form>")

print("</body>")
print("</html>")
