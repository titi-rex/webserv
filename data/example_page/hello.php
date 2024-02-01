#!/usr/bin/php-cgi
<?php
// En-tête HTTP indiquant que la réponse est en HTML
header("Content-Type: text/html");

// Contenu de la page
echo "<!DOCTYPE html>";
echo "<html>";
echo "<head>";
echo "<title>Exemple de page PHP</title>";
echo "</head>";
echo "<body>";
echo "<h1>Bienvenue sur cette page PHP générée par PHP CGI</h1>";
echo "<p>Cette page affiche du contenu HTML généré dynamiquement par PHP.</p>";
echo "<p>La date et l'heure actuelles sont : " . date("d-m-Y H:i:s") . "</p>";
echo "</body>";
echo "</html>";
?>
