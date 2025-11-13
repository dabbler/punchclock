# Backend API Endpoint for Accumulated Hours

This directory contains example backend code needed to support the new "accumulated hours" feature.

## Feature Overview

The punchclock application now displays:
- Current session hours
- Total accumulated hours for today on the current project
- This includes all previous clock in/out sessions today plus the current session (if clocked in)

## Backend Requirements

You need to deploy the `todayhours.php` endpoint (or equivalent) to your backend server.

### Endpoint Specification

**URL:** `/todayhours.php`

**Method:** GET

**Parameters:**
- `login` - Username (URL encoded)
- `project_id` - Integer project ID
- `date` - Date in format YYYY-MM-DD

**Response:**
- Plain text floating point number
- Example: `5.750`
- Represents total hours worked today on the specified project
- Should include only **completed** sessions (where end_time IS NOT NULL)
- Should NOT include the current ongoing session

### Example Response

```
3.50
```

This would indicate that the user has logged 3.5 hours today on the specified project in completed clock in/out sessions.

## Database Schema Assumptions

The example code assumes a database table structure similar to:

```sql
CREATE TABLE timecards (
    id INT PRIMARY KEY AUTO_INCREMENT,
    login VARCHAR(255),
    project_id INT,
    start_time DATETIME,
    end_time DATETIME,
    hours FLOAT,
    description TEXT
);
```

Adjust the SQL query in `todayhours.php` to match your actual database schema.

## Installation

1. Review and modify `todayhours.php` to match your database configuration
2. Deploy `todayhours.php` to your backend server (same location as other punchclock PHP files)
3. Ensure proper database permissions for the query
4. Test the endpoint manually before using with the client

## Testing

Test the endpoint manually:

```bash
curl "http://yourserver.com/todayhours.php?login=testuser&project_id=1&date=2025-11-13"
```

Should return a numeric value like `3.50` or `0.0`
