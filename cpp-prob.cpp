showModalBottomSheet(
  context: context,
  isScrollControlled: true,
  builder: (_) {
    return Padding(
      padding: EdgeInsets.only(bottom: MediaQuery.of(context).viewInsets.bottom),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          ListTile(title: Text(request.customerName ?? "Unknown")),
          ListTile(leading: Icon(Icons.my_location), title: Text(request.pickupAddress ?? '')),
          ListTile(leading: Icon(Icons.location_on), title: Text(request.dropoffAddress ?? '')),
          ButtonBar(
            children: [
              ElevatedButton(onPressed: () { /* decline */ }, child: Text('Decline')),
              ElevatedButton(onPressed: () { /* accept */ }, child: Text('Accept')),
            ],
          ),
        ],
      ),
    );
  },
);
