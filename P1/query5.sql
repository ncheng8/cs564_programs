SELECT COUNT(*)
FROM Users u
WHERE u.rating > 1000
AND u.userID IN (SELECT i.sellerID
                 FROM Items i)