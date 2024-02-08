<?php

echo "<!DOCTYPE html>";
echo "<html>";
echo "<head>";
echo "<h1>Entrée fichier php</h1>";
echo "<p>";

if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_FILES["fileToUpload"])) {
    $targetDirectory = "uploads/";
    $targetFile = $targetDirectory . basename($_FILES["fileToUpload"]["name"]);

    if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $targetFile)) {
        echo "Le fichier " . basename($_FILES["fileToUpload"]["name"]) . " a été téléchargé avec succès.";
    } else {
        echo "Une erreur s'est produite lors du téléchargement du fichier.";
    }
}

echo "</p>";
echo "</body>";
echo "</html>";

?>
