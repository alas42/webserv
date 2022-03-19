<!DOCTYPE html>

<html>
	<body>
		<form action="php-cgi" method="post">
			Name: <input type="text" name="name"><br>
			Age: <input type="text" name="age"><br>
			Date of birth: <input type="text" name="date_of_birth"><br>
			Faction: <input type="text" name="faction"><br>
			<input type="submit">
		</form>
		<?php
			print_r($_POST);
		?>
	</body>
</html>
