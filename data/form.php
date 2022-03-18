<!DOCTYPE html>

<html>
	<body>
		<form action="php-cgi" method="post">
			Name: <input type="text" name="name"><br>
			E-mail: <input type="text" name="email"><br>
			<input type="submit">
		</form>
		<?php
			if(isset($_POST['name'])) {
				echo 'Hello ' . htmlspecialchars($_POST["name"]) . '!';
			}
		?>
	</body>
</html>
