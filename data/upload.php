<?php
	$target_dir = "uploads/";
	$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
	$uploadOk = 1;
	$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));

	// Check if image file is a actual image or fake image
	if(isset($_POST["submit"]))
	{
		$uploadOk = 1;
	}
	else
	{
		echo "not isset\n";
	}

	// Check if file already exists
	if (file_exists($target_file))
	{
		echo "Sorry, file already exists.\n";
		$uploadOk = 0;
	}

	// Check file size
	if ($_FILES["fileToUpload"]["size"] > 5000000)
	{
		echo "Sorry, your file is too large.\n";
		$uploadOk = 0;
	}

	// Allow certain file formats
	if($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg"
	&& $imageFileType != "gif" && $imageFileType != 'html' && $imageFileType != "txt")
	{
		echo "Sorry, only JPG, JPEG, PNG, GIF, HTML & TXT files are allowed.\n";
		$uploadOk = 0;
	}

	// Check if $uploadOk is set to 0 by an error
	if ($uploadOk == 0)
	{
		echo "Sorry, your file was not uploaded.\n";
	}
	else
	{
		if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file))
		{
			echo "The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.\n";
		}
		else
		{
			echo "Sorry, there was an error uploading your file.\n";
		}
	}
?>