<!DOCTYPE html>

<html>
	<body>
		<form action="php-cgi" method="post">
			Name: <input type="text" name="name"><br>
			<input type="submit">
		</form>
		<?php
			print_r($_POST);
		?>
	</body>
</html>
