<!DOCTYPE html>

<html>
	<body>
		<form action="php-cgi" method="get">
			Name: <input type="text" name="name"><br>
			E-mail: <input type="text" name="email"><br>
			<input type="submit">
		</form>
		<?php
			if(isset($_GET['name'])) {
				echo 'Bonjour ' . htmlspecialchars($_GET["name"]) . '!';
			}
			if (isset($_GET['email'])){
				echo 'Your email is ' . htmlspecialchars($_GET["name"]). '.';
			}
		?>
	</body>
</html>
