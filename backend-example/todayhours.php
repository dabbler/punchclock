<?php
/*
 * Backend API endpoint to return today's accumulated hours for a specific user and project
 *
 * This file should be deployed to your backend server at the location specified by glbHostUrl
 *
 * Expected parameters:
 *   - login: username (URL encoded)
 *   - project_id: the project ID
 *   - date: date in format YYYY-MM-DD (today's date)
 *
 * Expected output:
 *   - A single floating point number representing the total hours worked today on this project
 *   - This should include all completed clock in/out sessions for today, but NOT the current session if still clocked in
 *
 * Example implementation:
 */

// Get parameters
$login = isset($_GET['login']) ? $_GET['login'] : '';
$project_id = isset($_GET['project_id']) ? intval($_GET['project_id']) : 0;
$date = isset($_GET['date']) ? $_GET['date'] : date('Y-m-d');

// Example database query (adjust to your schema):
// SELECT SUM(hours) FROM timecards
// WHERE login = ? AND project_id = ? AND DATE(start_time) = ? AND end_time IS NOT NULL

// For now, return example data
// In production, replace this with actual database query

/*
// Example PDO query:
try {
    $pdo = new PDO('mysql:host=localhost;dbname=timecards', 'username', 'password');
    $stmt = $pdo->prepare("
        SELECT SUM(hours) as total_hours
        FROM timecards
        WHERE login = :login
        AND project_id = :project_id
        AND DATE(start_time) = :date
        AND end_time IS NOT NULL
    ");
    $stmt->execute([
        'login' => $login,
        'project_id' => $project_id,
        'date' => $date
    ]);
    $result = $stmt->fetch(PDO::FETCH_ASSOC);

    $total_hours = $result['total_hours'] ? floatval($result['total_hours']) : 0.0;
    echo number_format($total_hours, 3, '.', '');

} catch (PDOException $e) {
    // Log error and return 0
    error_log("Error querying today's hours: " . $e->getMessage());
    echo "0.0";
}
*/

// For testing purposes, return 0.0
// Replace this with your actual database query
echo "0.0";

?>
