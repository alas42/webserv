<!DOCTYPE html>

<html>
	<body>
		<form action="/post_form.php" method="post">
			Name: <input type="text" name="name"><br><br>
			Age: <input type="text" name="age"><br><br>
			Date of birth: <input type="text" name="date_of_birth"><br><br>
			Faction: <input type="text" name="faction"><br><br>
			<input type="submit">
		</form>
		<br>

		<br>
		<?php
			print_r($_POST);
		?>
	</body>
</html>
