<!DOCTYPE html>

<html>
	<body>
		<form action="./get_form.php" method="get">
			Name: <input type="text" name="name" maxlength="1000"><br>
			E-mail: <input type="text" name="email" maxlength="1000"><br>
			<input type="submit">
		</form>
		<?php
			if(isset($_GET['name'])) {
				echo 'Bonjour ' . htmlspecialchars($_GET["name"]) . '!';
			}
			if (isset($_GET['email'])){
				echo 'Your email is ' . htmlspecialchars($_GET["email"]). '.';
			}
		?>
	</body>
</html>
