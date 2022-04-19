<?php

	$target_dir = "./" . $_SERVER["UPLOAD_STORE"] . "/";
	$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
	$uploadOk = 0;
	$imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));

	echo "<!DOCTYPE html>";

	if(isset($_POST["submit"]))
	{
		$uploadOk = 1;
	}
	else
	{
		echo "Error\n" . "<br>";
	}

	if($_FILES["fileToUpload"]["size"] <= 0)
	{
		$uploadOk = 0;
		echo "Error\n" . "<br>";
		echo "Please, choose a file\n"  . "<br>";
	}

	// Check if file already exists
	if ($uploadOk == 1 && file_exists($target_file))
	{
		echo "Error\n" ."<br>";
		echo htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])) . " already exists.\n";
		$uploadOk = 0;
	}
	// Check file size
	if ($uploadOk == 1 && $_FILES["fileToUpload"]["size"] > 1000000000)
	{
		echo "Error\n" ."<br>";
		echo htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])) . " is too large.\n";
		$uploadOk = 0;
	}

	// Check if $uploadOk is set to 0 by an error
	if ($uploadOk == 0)
	{
		echo htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])) . " was not uploaded.\n";
	}
	else
	{
		if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file))
		{
			echo "Success\n" ."<br>";
			echo htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.\n";
		}
		else
		{
			echo "Error\n" ."<br>";
			echo htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])) . ": uploading file did not work.\n";
		}
	}
?>
