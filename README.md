# ITI-Tasks
# Driver Home Screen — README

Engineer — here’s a focused README for the **Driver Home Screen** feature (`home_page_driver_screen.dart`). It documents purpose, behavior, integration points, dependencies, how to run, testing notes, and a checklist of what may still need wiring or completion.

---

# Overview

This screen is the main entry point for driver users. Responsibilities:

* Show map and driver-related overlays.
* Display incoming trip requests (horizontal card list).
* Let driver open request details in a bottom sheet and perform actions (accept/decline, navigate to customer, start trip, etc.).
* Maintain driver state (online/offline) and periodically refresh trip requests.
* Integrate with app-wide BLoCs and services (map, ride, driver, trip-requests).
* Handle lifecycle events (resume/pause) and app-exit double-back.

File(s):

* `home_page_driver_screen.dart` — main screen (refactored version stored in canvas as **Home Page Driver Screen Refactored**).

---

# Key features & behavior

* Initializes required BLoCs on first frame:

  * `DriverTripRequestBloc` — fetches/holds driver trip requests.
  * `DriverBloc` — driver profile and status.
  * `MapBloc` / `RideBloc` — map & ride-related actions.
* Starts a periodic timer to refresh trip requests (e.g., every 30s).
* On app resume, triggers a refresh.
* Displays a horizontal list of trip request cards. Tapping a card shows a modal bottom sheet with request details.
* Double-back to exit (toast on first press).
* Drawer and notifications access via top bar.

---

# Architecture & flow

1. **Screen** gets injected BLoCs via `BlocProvider`.
2. `initState` dispatches initial `FetchTripRequests` and `FetchDriverData`.
3. A periodic timer dispatches `FetchTripRequests` regularly.
4. UI reacts to `DriverTripRequestState` and `MapState` via `BlocBuilder`.
5. Tapping a request calls `_showTripRequestDetails(request)` which opens a modal bottom sheet; actions inside sheet dispatch corresponding BLoC events (e.g., `AcceptTripRequest`, `DeclineTripRequest`, `StartRide`).

---

# Important files / widgets

* `home_page_driver_screen.dart` — main (refactored).
* `AppDrawer` — app drawer referenced by `drawer` property.
* `NotificationScreen` — screen for notifications.
* BLoC files referenced:

  * `driver_trip_request_bloc` + events + states
  * `driver_bloc`, `map_bloc`, `ride_bloc` etc.
* Model: `DriverTripRequest` — used by the request list and details.

---

# External packages & dependencies

The refactor references these packages (ensure they're in `pubspec.yaml`):

* `flutter_bloc`
* `easy_localization`
* `location`
* `device_info_plus`
* `dio`
* `fluttertoast`
* `flutter_screenutil`
* `flutter_svg`
* plus app internal packages (utils, network service, storage, etc.)

Add any missing internal imports to your project’s packages if not already present.

---

# Integration guide

1. **Provide required BLoCs**
   Ensure upper widget tree has `BlocProvider`s for:

   ```dart
   BlocProvider(
     create: (_) => DriverTripRequestBloc(...),
     child: BlocProvider(
       create: (_) => DriverBloc(...),
       child: BlocProvider(
         create: (_) => MapBloc(...),
         child: HomePageDriverScreen(),
       ),
     ),
   )
   ```

2. **Model contract**
   `DriverTripRequest` should expose:

   * `String? customerName`
   * `String? pickupAddress`
   * `String? dropoffAddress`
   * any IDs/fields used in actions (price, distance, coordinates).

3. **BLoC events (examples)**
   The screen expects events similar to:

   ```dart
   driverTripRequestBloc.add(FetchTripRequests(refresh: true));
   driverBloc.add(FetchDriverData());
   // When accepting a trip (example inside bottom sheet)
   driverTripRequestBloc.add(AcceptTripRequest(tripId: request.id));
   ```

4. **Navigation**

   * Drawer: `AppDrawer` must exist in project.
   * Notifications: `NotificationScreen`.
   * Ride flow: screens such as `DriverInTripScreen` should handle active rides.

5. **Permissions**

   * Location permission is required. Make sure to request and handle location permissions before showing/updating map & trips.
   * Add permissions in `AndroidManifest.xml` and `Info.plist` as needed.

---

# Platform & permission setup

Android `AndroidManifest.xml`:

```xml
<uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
<uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />
```

iOS `Info.plist`:

```xml
<key>NSLocationWhenInUseUsageDescription</key>
<string>We need your location to provide rides and navigation.</string>
```

If you use background location updates, add background-permission entries and explain them to Apple.

---

# How to run & verify locally

1. `flutter pub get`
2. Ensure all BLoC providers are registered in widget tree.
3. Run app and navigate to Driver Home Screen.
4. Check logs for:

   * Missing imports or types (resolve by adding missing files).
   * Timer starts/stops properly when entering/exiting the screen.
   * Bottom sheet opens when tapping a request.

Run analyzer:

```bash
dart analyze
flutter analyze
```

---

# Testing recommendations

* **Unit tests** for BLoCs:

  * Test `FetchTripRequests`, `AcceptTripRequest`, `DeclineTripRequest`.
* **Widget tests** for screen:

  * Verify initial fetch dispatch.
  * Mock `DriverTripRequestBloc` with known states and check that correct UI (cards/list) renders.
  * Simulate tap on card and assert bottom sheet appears.
* **Integration tests**:

  * Full flow: receive trip -> accept -> navigate to `DriverInTripScreen`.

---

# Troubleshooting & common issues

* **Missing imports / types**: If analyzer complains, add or correct local imports (models, blocs, util classes).
* **BLoC not provided**: Error `BlocProvider not found` — wrap a parent widget with required `BlocProvider`.
* **Timers after dispose**: Make sure `_periodicTimer?.cancel()` is present in `dispose()` (it is in the refactor). If timers still run, confirm `mounted` checks inside callbacks.
* **Bottom sheet context null**: If you programmatically close the bottom sheet, null-check `_bottomSheetContext`.

---

# Security & performance notes

* Avoid heavy work inside the UI thread. Keep heavy network calls inside repository/network classes or BLoCs.
* Cancel subscriptions and timers in `dispose` to avoid memory leaks.
* Consider debouncing or exponential backoff for network polling in case of server issues.

---

# TODO / Known placeholders (what I left intentionally simplified)

* The bottom sheet currently contains a placeholder `Text(...)`. Replace it with full request details and action buttons. Example actions to add:

  * Accept / Decline request
  * Navigate to pickup
  * Call customer
  * Show price & ETA
* Add more precise periodic timing or replace `Timer.periodic` with a scheduling service if desired.
* Wire `SoundManagerHelper` and other platform-specific helpers to play request sounds and handle accountability.
* Add analytics events for important flows (request received, request accepted, trip started).

---

# Example bottom-sheet skeleton (place inside `_showTripRequestDetails`)

```dart
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
```

---

# Checklist before shipping

* [ ] Replace bottom-sheet placeholder with full UI and actions.
* [ ] Confirm `DriverTripRequestBloc` events & payload match backend contract.
* [ ] Ensure location permissions are requested and honored.
* [ ] Add tests for main flows.
* [ ] Confirm design / style matches the rest of the app (theme, ScreenUtil usage).
* [ ] Validate on device (Android & iOS) — especially location & background behavior.

---

# Contact & next steps

If you want, I can:

* Fill in the bottom-sheet UI and wire accept/decline actions while keeping logic unchanged.
* Produce a line-by-line checklist of missing models/imports (based on `dart analyze` output) — paste the analyzer output and I’ll map fixes.
* Create widget tests that mock the BLoCs and assert UI behavior.

Tell me which one you prefer and I’ll continue.
