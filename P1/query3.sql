SELECT COUNT(*)
FROM (SELECT COUNT(*) AS numCat
      FROM Items i, Categories c
      WHERE i.itemID = c.itemID
      GROUP BY i.itemID) AS m
WHERE m.numCat = 4